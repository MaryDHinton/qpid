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
 * \file EventHandleImpl.h
 */

#ifndef qpid_asyncStore_EventHandleImpl_h_
#define qpid_asyncStore_EventHandleImpl_h_

#include "qpid/RefCounted.h"

namespace qpid {

namespace broker {
class QueueHandle;
}

namespace asyncStore {

class EventHandleImpl : public virtual qpid::RefCounted
{
public:
    EventHandleImpl(qpid::broker::QueueHandle& queueHandle,
                    const std::string& key = std::string());
    virtual ~EventHandleImpl();
    const std::string& getKey() const;
protected:
    qpid::broker::QueueHandle& m_queueHandle;
    std::string m_key;
};

}} // namespace qpid::asyncStore

#endif // qpid_asyncStore_EventHandleImpl_h_