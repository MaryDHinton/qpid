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
 * \file SimpleTxnPublish.cpp
 */

#include "SimpleTxnPublish.h"

#include "SimpleMessage.h"
#include "SimpleQueue.h"
#include "SimpleQueuedMessage.h"

#include "qpid/log/Statement.h"
#include <boost/make_shared.hpp>

namespace qpid {
namespace broker {

SimpleTxnPublish::SimpleTxnPublish(boost::intrusive_ptr<SimpleMessage> msg) :
        m_msg(msg)
{}

SimpleTxnPublish::~SimpleTxnPublish() {}

bool
SimpleTxnPublish::prepare(SimpleTxnBuffer* tb) throw() {
    try {
        while (!m_queues.empty()) {
            m_queues.front()->prepareEnqueue(tb);
            m_prepared.push_back(m_queues.front());
            m_queues.pop_front();
        }
        return true;
    } catch (const std::exception& e) {
        QPID_LOG(error, "TxnPublish: Failed to prepare transaction: " << e.what());
    } catch (...) {
        QPID_LOG(error, "TxnPublish: Failed to prepare transaction: (unknown error)");
    }
    return false;
}

void
SimpleTxnPublish::commit() throw() {
    try {
        for (std::list<boost::shared_ptr<SimpleQueuedMessage> >::iterator i = m_prepared.begin(); i != m_prepared.end(); ++i) {
            (*i)->commitEnqueue();
        }
    } catch (const std::exception& e) {
        QPID_LOG(error, "TxnPublish: Failed to commit transaction: " << e.what());
    } catch (...) {
        QPID_LOG(error, "TxnPublish: Failed to commit transaction: (unknown error)");
    }
}

void
SimpleTxnPublish::rollback() throw() {
    try {
        for (std::list<boost::shared_ptr<SimpleQueuedMessage> >::iterator i = m_prepared.begin(); i != m_prepared.end(); ++i) {
            (*i)->abortEnqueue();
        }
    } catch (const std::exception& e) {
        QPID_LOG(error, "TxnPublish: Failed to rollback transaction: " << e.what());
    } catch (...) {
        QPID_LOG(error, "TxnPublish: Failed to rollback transaction: (unknown error)");
    }
}

uint64_t
SimpleTxnPublish::contentSize() {
    return m_msg->contentSize();
}

void
SimpleTxnPublish::deliverTo(const boost::shared_ptr<SimpleQueue>& queue) {
    m_queues.push_back(boost::shared_ptr<SimpleQueuedMessage>(new SimpleQueuedMessage(queue.get(), m_msg)));
    m_delivered = true;
}

SimpleMessage&
SimpleTxnPublish::getMessage() {
    return *m_msg;
}

}} // namespace qpid::broker