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
 * \file TransactionAsyncContext.cpp
 */

#include "TxnAsyncContext.h"

namespace qpid {
namespace broker {

TxnAsyncContext::TxnAsyncContext(SimpleTxnBuffer* const tb,
                                 AsyncResultCallback rcb,
                                 AsyncResultQueue* const arq):
        m_tb(tb),
        m_rcb(rcb),
        m_arq(arq)
{}

TxnAsyncContext::~TxnAsyncContext() {}

SimpleTxnBuffer*
TxnAsyncContext::getTxnBuffer() const {
    return m_tb;
}

AsyncResultQueue*
TxnAsyncContext::getAsyncResultQueue() const {
    return m_arq;
}

void
TxnAsyncContext::invokeCallback(const AsyncResultHandle* const arh) const {
    if (m_rcb) {
        m_rcb(arh);
    }
}

}} // namespace qpid::broker