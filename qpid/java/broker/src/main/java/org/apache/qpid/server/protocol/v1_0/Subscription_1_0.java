/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */
package org.apache.qpid.server.protocol.v1_0;

import org.apache.qpid.amqp_1_0.transport.SendingLinkEndpoint;
import org.apache.qpid.amqp_1_0.type.Binary;
import org.apache.qpid.amqp_1_0.type.DeliveryState;
import org.apache.qpid.amqp_1_0.type.Outcome;
import org.apache.qpid.amqp_1_0.type.messaging.Accepted;
import org.apache.qpid.amqp_1_0.type.messaging.Released;
import org.apache.qpid.amqp_1_0.type.messaging.Source;
import org.apache.qpid.amqp_1_0.type.messaging.StdDistMode;
import org.apache.qpid.amqp_1_0.type.transaction.TransactionalState;
import org.apache.qpid.amqp_1_0.type.transport.SenderSettleMode;
import org.apache.qpid.amqp_1_0.type.transport.Transfer;

import org.apache.qpid.AMQException;
import org.apache.qpid.server.logging.LogActor;
import org.apache.qpid.server.message.ServerMessage;
import org.apache.qpid.server.queue.AMQQueue;
import org.apache.qpid.server.queue.QueueEntry;
import org.apache.qpid.server.subscription.Subscription;
import org.apache.qpid.server.txn.ServerTransaction;

import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReentrantLock;

class Subscription_1_0 implements Subscription
{
    private SendingLink_1_0 _link;

    private AMQQueue _queue;

    private final AtomicReference<State> _state = new AtomicReference<State>(State.SUSPENDED);

    private final QueueEntry.SubscriptionAcquiredState _owningState = new QueueEntry.SubscriptionAcquiredState(this);
    private final QueueEntry.SubscriptionAssignedState _assignedState = new QueueEntry.SubscriptionAssignedState(this);
    private final long _id;
    private final boolean _acquires;
    private AMQQueue.Context _queueContext;
    private Map<String, Object> _properties = new ConcurrentHashMap<String, Object>();
    private ReentrantLock _stateChangeLock = new ReentrantLock();

    private long _deliveryTag = 0L;
    private StateListener _stateListener;

    private Binary _transactionId;

    public Subscription_1_0(final SendingLink_1_0 link, final QueueDestination destination)
    {
        _link = link;
        _queue = destination.getQueue();
        _id = getEndpoint().getLocalHandle().longValue();
        _acquires = !StdDistMode.COPY.equals(((Source) (getEndpoint().getSource())).getDistributionMode());
    }

    private SendingLinkEndpoint getEndpoint()
    {
        return _link.getEndpoint();
    }

    public LogActor getLogActor()
    {
        return null;  //TODO
    }

    public boolean isTransient()
    {
        return true;  //TODO
    }

    public AMQQueue getQueue()
    {
        return _queue;
    }

    public QueueEntry.SubscriptionAcquiredState getOwningState()
    {
        return _owningState;
    }

    public QueueEntry.SubscriptionAssignedState getAssignedState()
    {
        return _assignedState;
    }

    public void setQueue(final AMQQueue queue, final boolean exclusive)
    {
        //TODO
    }

    public void setNoLocal(final boolean noLocal)
    {
        //TODO
    }

    public long getSubscriptionID()
    {
        return _id;
    }

    public boolean isSuspended()
    {
        final boolean isSuspended = !isActive();// || !getEndpoint().hasCreditToSend();
        return isSuspended;
    }

    public boolean hasInterest(final QueueEntry msg)
    {
        return true;  //TODO - filters
    }

    public boolean isClosed()
    {
        return !getEndpoint().isAttached();
    }

    public boolean acquires()
    {
        return _acquires;
    }

    public boolean seesRequeues()
    {
        // TODO
        return acquires();
    }

    public void close()
    {
        getEndpoint().detach();
    }

    public void send(final QueueEntry queueEntry) throws AMQException
    {
        //TODO
        ServerMessage serverMessage = queueEntry.getMessage();
        if(serverMessage instanceof Message_1_0)
        {
            Message_1_0 message = (Message_1_0) serverMessage;
            Transfer transfer = new Transfer();
            //TODO


            List<ByteBuffer> fragments = message.getFragments();
            ByteBuffer payload;
            if(fragments.size() == 1)
            {
                payload = fragments.get(0);
            }
            else
            {
                int size = 0;
                for(ByteBuffer fragment : fragments)
                {
                    size += fragment.remaining();
                }

                payload = ByteBuffer.allocate(size);

                for(ByteBuffer fragment : fragments)
                {
                    payload.put(fragment.duplicate());
                }

                payload.flip();
            }

            transfer.setPayload(payload);
            byte[] data = new byte[8];
            ByteBuffer.wrap(data).putLong(_deliveryTag++);
            final Binary tag = new Binary(data);

            transfer.setDeliveryTag(tag);

            synchronized(_link.getLock())
            {
                if(_link.isAttached())
                {
                    if(SenderSettleMode.SETTLED.equals(getEndpoint().getSendingSettlementMode()))
                    {
                        transfer.setSettled(true);
                    }
                    else
                    {
                        UnsettledAction action = _acquires
                                                 ? new DispositionAction(tag, queueEntry)
                                                 : new DoNothingAction(tag, queueEntry);

                        _link.addUnsettled(tag, action, queueEntry);
                    }

                    if(_transactionId != null)
                    {
                        TransactionalState state = new TransactionalState();
                        state.setTxnId(_transactionId);
                        transfer.setState(state);
                    }
                    // TODO - need to deal with failure here
                    if(_acquires && _transactionId != null)
                    {
                        ServerTransaction txn = _link.getTransaction(_transactionId);
                        if(txn != null)
                        {
                            txn.addPostTransactionAction(new ServerTransaction.Action(){

                                public void postCommit()
                                {
                                    //To change body of implemented methods use File | Settings | File Templates.
                                }

                                public void onRollback()
                                {
                                    if(queueEntry.isAcquiredBy(Subscription_1_0.this))
                                    {
                                        queueEntry.release();
                                        _link.getEndpoint().updateDisposition(tag, (DeliveryState)null, true);


                                    }
                                }
                            });
                        }

                    }

                    getEndpoint().transfer(transfer);
                }
                else
                {
                    queueEntry.release();
                }
            }
        }

    }

    public void queueDeleted(final AMQQueue queue)
    {
        //TODO
        getEndpoint().setSource(null);
        getEndpoint().detach();
    }

    public boolean wouldSuspend(final QueueEntry msg)
    {
        final boolean hasCredit = _link.isAttached() && getEndpoint().hasCreditToSend();
        if(!hasCredit && getState() == State.ACTIVE)
        {
            if(_state.compareAndSet(State.ACTIVE, State.SUSPENDED))
            {
                _stateListener.stateChange(this, State.ACTIVE, State.SUSPENDED);
            }
        }

        return !hasCredit;
    }

    public void suspend()
    {
        if(_state.compareAndSet(State.ACTIVE, State.SUSPENDED))
        {
            _stateListener.stateChange(this, State.ACTIVE, State.SUSPENDED);
        }
    }

    public void getSendLock()
    {
        _stateChangeLock.lock();
    }

    public void releaseSendLock()
    {
        _stateChangeLock.unlock();
    }


    public void onDequeue(final QueueEntry queueEntry)
    {
        //TODO
    }

    public void restoreCredit(final QueueEntry queueEntry)
    {
        //TODO
    }

    public void setStateListener(final StateListener listener)
    {
        _stateListener = listener;
    }

    public State getState()
    {
        return _state.get();
    }

    public AMQQueue.Context getQueueContext()
    {
        return _queueContext;
    }

    public void setQueueContext(AMQQueue.Context queueContext)
    {
        _queueContext = queueContext;
    }


    public boolean isActive()
    {
        return getState() == State.ACTIVE;
    }

    public void set(String key, Object value)
    {
        _properties.put(key, value);
    }

    public Object get(String key)
    {
        return _properties.get(key);
    }

    public boolean isSessionTransactional()
    {
        return false;  //TODO
    }

    public void queueEmpty()
    {
        if(_link.drained())
        {
            if(_state.compareAndSet(State.ACTIVE, State.SUSPENDED))
            {
                _stateListener.stateChange(this, State.ACTIVE, State.SUSPENDED);
            }
        }
    }

    public void flowStateChanged()
    {
        if(isSuspended())
        {
            if(_state.compareAndSet(State.SUSPENDED, State.ACTIVE))
            {
                _stateListener.stateChange(this, State.SUSPENDED, State.ACTIVE);
            }
            _transactionId = _link.getTransactionId();
        }
    }

    private class DispositionAction implements UnsettledAction
    {

        private final QueueEntry _queueEntry;
        private final Binary _deliveryTag;

        public DispositionAction(Binary tag, QueueEntry queueEntry)
        {
            _deliveryTag = tag;
            _queueEntry = queueEntry;
        }

        public boolean process(DeliveryState state, Boolean settled)
        {

            Binary transactionId = null;
            final Outcome outcome;
            // If disposition is settled this overrides the txn?
            if(state instanceof TransactionalState)
            {
                transactionId = ((TransactionalState)state).getTxnId();
                outcome = ((TransactionalState)state).getOutcome();
            }
            else if (state instanceof Outcome)
            {
                outcome = (Outcome) state;
            }
            else
            {
                outcome = null;
            }


            ServerTransaction txn = _link.getTransaction(transactionId);

            if(outcome instanceof Accepted)
            {
                txn.dequeue(_queueEntry.getQueue(), _queueEntry.getMessage(),
                        new ServerTransaction.Action()
                        {

                            public void postCommit()
                            {
                                if(_queueEntry.isAcquiredBy(Subscription_1_0.this))
                                {
                                    _queueEntry.discard();
                                }
                            }

                            public void onRollback()
                            {

                            }
                        });
                txn.addPostTransactionAction(new ServerTransaction.Action()
                    {
                        public void postCommit()
                        {
                            //_link.getEndpoint().settle(_deliveryTag);
                            _link.getEndpoint().updateDisposition(_deliveryTag, (DeliveryState)outcome, true);
                            _link.getEndpoint().sendFlowConditional();
                        }

                        public void onRollback()
                        {
                        }
                    });
            }
            else if(outcome instanceof Released)
            {
                txn.addPostTransactionAction(new ServerTransaction.Action()
                {
                    public void postCommit()
                    {
                        _queueEntry.release();
                        _link.getEndpoint().settle(_deliveryTag);
                    }

                    public void onRollback()
                    {
                        _link.getEndpoint().settle(_deliveryTag);
                    }
                });
            }

            return (transactionId == null && outcome != null);
        }
    }

    private class DoNothingAction implements UnsettledAction
    {
        public DoNothingAction(final Binary tag,
                               final QueueEntry queueEntry)
        {
        }

        public boolean process(final DeliveryState state, final Boolean settled)
        {
            Binary transactionId = null;
            Outcome outcome = null;
            // If disposition is settled this overrides the txn?
            if(state instanceof TransactionalState)
            {
                transactionId = ((TransactionalState)state).getTxnId();
                outcome = ((TransactionalState)state).getOutcome();
            }
            else if (state instanceof Outcome)
            {
                outcome = (Outcome) state;
            }
            return true;
        }
    }
}