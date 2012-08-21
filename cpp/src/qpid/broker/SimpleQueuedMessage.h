/*
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
 */

/**
 * \file SimpleQueuedMessage.h
 */

#ifndef qpid_broker_SimpleQueuedMessage_h_
#define qpid_broker_SimpleQueuedMessage_h_

#include "AsyncStore.h"
#include "EnqueueHandle.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/intrusive_ptr.hpp>

namespace qpid {
namespace broker {

class SimpleMessage;
class SimpleQueue;

class SimpleQueuedMessage : public boost::enable_shared_from_this<SimpleQueuedMessage>
{
public:
    SimpleQueuedMessage();
    SimpleQueuedMessage(SimpleQueue* q,
                  boost::intrusive_ptr<SimpleMessage> msg);
    SimpleQueuedMessage(const SimpleQueuedMessage& qm);
    SimpleQueuedMessage(SimpleQueuedMessage* const qm);
    virtual ~SimpleQueuedMessage();
    SimpleQueue* getQueue() const;
    boost::intrusive_ptr<SimpleMessage> payload() const;
    const EnqueueHandle& enqHandle() const;
    EnqueueHandle& enqHandle();

    // --- Transaction handling ---
    void prepareEnqueue(qpid::broker::SimpleTxnBuffer* tb);
    void commitEnqueue();
    void abortEnqueue();

private:
    SimpleQueue* m_queue;
    boost::intrusive_ptr<SimpleMessage> m_msg;
    qpid::broker::EnqueueHandle m_enqHandle;
};

}} // namespace qpid::broker

#endif // qpid_broker_SimpleQueuedMessage_h_