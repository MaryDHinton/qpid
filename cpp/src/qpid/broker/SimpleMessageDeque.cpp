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
 * \file SimpleMessageDeque.cpp
 */

#include "SimpleMessageDeque.h"

#include "SimpleQueuedMessage.h"

namespace qpid  {
namespace broker {

SimpleMessageDeque::SimpleMessageDeque() {}

SimpleMessageDeque::~SimpleMessageDeque() {}

uint32_t
SimpleMessageDeque::size() {
    qpid::sys::ScopedLock<qpid::sys::Mutex> l(m_msgMutex);
    return m_messages.size();
}

bool
SimpleMessageDeque::push(boost::shared_ptr<SimpleQueuedMessage>& added) {
    qpid::sys::ScopedLock<qpid::sys::Mutex> l(m_msgMutex);
    m_messages.push_back(added);
    return false;
}

bool
SimpleMessageDeque::consume(boost::shared_ptr<SimpleQueuedMessage>& msg) {
    qpid::sys::ScopedLock<qpid::sys::Mutex> l(m_msgMutex);
    if (!m_messages.empty()) {
        msg = m_messages.front();
        m_messages.pop_front();
        return true;
    }
    return false;
}

}} // namespace qpid::broker