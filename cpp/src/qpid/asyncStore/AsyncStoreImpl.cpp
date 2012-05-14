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
 * \file AsyncStoreImpl.cpp
 */

#include "AsyncStoreImpl.h"

#include "AsyncOperation.h"

#include "qpid/broker/ConfigHandle.h"
#include "qpid/broker/EnqueueHandle.h"
#include "qpid/broker/EventHandle.h"
#include "qpid/broker/MessageHandle.h"
#include "qpid/broker/QueueHandle.h"
#include "qpid/broker/TxnHandle.h"

#include <boost/intrusive_ptr.hpp>

namespace qpid {
namespace asyncStore {

AsyncStoreImpl::AsyncStoreImpl(boost::shared_ptr<qpid::sys::Poller> poller,
                               const AsyncStoreOptions& opts) :
        m_poller(poller),
        m_opts(opts),
        m_runState(),
        m_operations(m_poller)
{}

AsyncStoreImpl::~AsyncStoreImpl()
{}

void
AsyncStoreImpl::initialize()
{}

uint64_t
AsyncStoreImpl::getNextRid()
{
    return m_ridCntr.next();
}

void
AsyncStoreImpl::initManagement(qpid::broker::Broker* /*broker*/)
{}

qpid::broker::TxnHandle
AsyncStoreImpl::createTxnHandle(const std::string& xid)
{
    return qpid::broker::TxnHandle(new TxnHandleImpl(xid));
}

qpid::broker::ConfigHandle
AsyncStoreImpl::createConfigHandle()
{
    return qpid::broker::ConfigHandle(new ConfigHandleImpl());
}

qpid::broker::QueueHandle
AsyncStoreImpl::createQueueHandle(const std::string& name,
                                  const qpid::types::Variant::Map& opts)
{
    return qpid::broker::QueueHandle(new QueueHandleImpl(name, opts));
}

qpid::broker::EventHandle
AsyncStoreImpl::createEventHandle(qpid::broker::QueueHandle& queueHandle,
                                  const std::string& key)
{
    return qpid::broker::EventHandle(new EventHandleImpl(queueHandle, key));
}

qpid::broker::MessageHandle
AsyncStoreImpl::createMessageHandle(const qpid::broker::DataSource* dataSrc)

{
    return qpid::broker::MessageHandle(new MessageHandleImpl(dataSrc));
}

qpid::broker::EnqueueHandle
AsyncStoreImpl::createEnqueueHandle(qpid::broker::MessageHandle& msgHandle,
                                    qpid::broker::QueueHandle& queueHandle)
{
    return qpid::broker::EnqueueHandle(new EnqueueHandleImpl(msgHandle, queueHandle));
}

void
AsyncStoreImpl::submitPrepare(qpid::broker::TxnHandle& txnHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::TXN_PREPARE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&txnHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitCommit(qpid::broker::TxnHandle& txnHandle,
                             qpid::broker::ResultCallback resultCb,
                             qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::TXN_COMMIT,
                                            dynamic_cast<qpid::broker::IdHandle*>(&txnHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitAbort(qpid::broker::TxnHandle& txnHandle,
                            qpid::broker::ResultCallback resultCb,
                            qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::TXN_ABORT,
                                            dynamic_cast<qpid::broker::IdHandle*>(&txnHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitCreate(qpid::broker::ConfigHandle& cfgHandle,
                             const qpid::broker::DataSource* dataSrc,
                             qpid::broker::ResultCallback resultCb,
                             qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::CONFIG_CREATE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&cfgHandle),
                                            dataSrc,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitDestroy(qpid::broker::ConfigHandle& cfgHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::CONFIG_DESTROY,
                                            dynamic_cast<qpid::broker::IdHandle*>(&cfgHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitCreate(qpid::broker::QueueHandle& queueHandle,
                             const qpid::broker::DataSource* dataSrc,
                             qpid::broker::ResultCallback resultCb,
                             qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::QUEUE_CREATE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&queueHandle),
                                            dataSrc,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitDestroy(qpid::broker::QueueHandle& queueHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::QUEUE_DESTROY,
                                            dynamic_cast<qpid::broker::IdHandle*>(&queueHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitFlush(qpid::broker::QueueHandle& queueHandle,
                            qpid::broker::ResultCallback resultCb,
                            qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::QUEUE_FLUSH,
                                            dynamic_cast<qpid::broker::IdHandle*>(&queueHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitCreate(qpid::broker::EventHandle& eventHandle,
                             const qpid::broker::DataSource* dataSrc,
                             qpid::broker::ResultCallback resultCb,
                             qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::EVENT_CREATE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&eventHandle),
                                            dataSrc,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitCreate(qpid::broker::EventHandle& eventHandle,
                             const qpid::broker::DataSource* dataSrc,
                             qpid::broker::TxnHandle& txnHandle,
                             qpid::broker::ResultCallback resultCb,
                             qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::EVENT_CREATE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&eventHandle),
                                            dataSrc,
                                            &txnHandle,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitDestroy(qpid::broker::EventHandle& eventHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::EVENT_DESTROY,
                                            dynamic_cast<qpid::broker::IdHandle*>(&eventHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitDestroy(qpid::broker::EventHandle& eventHandle,
                             qpid::broker::TxnHandle& txnHandle,
                             qpid::broker::ResultCallback resultCb,
                             qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::EVENT_DESTROY,
                                            dynamic_cast<qpid::broker::IdHandle*>(&eventHandle),
                                            &txnHandle,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitEnqueue(qpid::broker::EnqueueHandle& enqHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::MSG_ENQUEUE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&enqHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitEnqueue(qpid::broker::EnqueueHandle& enqHandle,
                              qpid::broker::TxnHandle& txnHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::MSG_ENQUEUE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&enqHandle),
                                            &txnHandle,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitDequeue(qpid::broker::EnqueueHandle& enqHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::MSG_DEQUEUE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&enqHandle),
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

void
AsyncStoreImpl::submitDequeue(qpid::broker::EnqueueHandle& enqHandle,
                              qpid::broker::TxnHandle& txnHandle,
                              qpid::broker::ResultCallback resultCb,
                              qpid::broker::BrokerAsyncContext* brokerCtxt)
{
    AsyncOperation* op = new AsyncOperation(AsyncOperation::MSG_DEQUEUE,
                                            dynamic_cast<qpid::broker::IdHandle*>(&enqHandle),
                                            &txnHandle,
                                            resultCb,
                                            brokerCtxt);
    m_operations.submit(op);
}

int
AsyncStoreImpl::loadContent(qpid::broker::MessageHandle& /*msgHandle*/,
                            qpid::broker::QueueHandle& /*queueHandle*/,
                            char* /*data*/,
                            uint64_t /*offset*/,
                            const uint64_t /*length*/)
{
    return 0;
}

}} // namespace qpid::asyncStore