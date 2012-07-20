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
 * \file TxnHandle.cpp
 */

#include "TxnHandle.h"

#include "PrivateImplRef.h"

#include "qpid/asyncStore/TxnHandleImpl.h"

namespace qpid {
namespace broker {

typedef PrivateImplRef<TxnHandle> PrivateImpl;

TxnHandle::TxnHandle(qpid::asyncStore::TxnHandleImpl* p) :
        Handle<qpid::asyncStore::TxnHandleImpl>()
{
    PrivateImpl::ctor(*this, p);
}

TxnHandle::TxnHandle(const TxnHandle& r) :
        Handle<qpid::asyncStore::TxnHandleImpl>()
{
    PrivateImpl::copy(*this, r);
}

TxnHandle::~TxnHandle()
{
    PrivateImpl::dtor(*this);
}

TxnHandle&
TxnHandle::operator=(const TxnHandle& r)
{
    return PrivateImpl::assign(*this, r);
}

// --- TxnHandleImpl methods ---

const std::string&
TxnHandle::getXid() const
{
    return impl->getXid();
}

bool
TxnHandle::is2pc() const
{
    return impl->is2pc();
}

void
TxnHandle::incrOpCnt()
{
    impl->incrOpCnt();
}

void
TxnHandle::decrOpCnt()
{
    impl->decrOpCnt();
}

}} // namespace qpid::broker