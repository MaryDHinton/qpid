#ifndef QPID_AMQP_0_10_CONTROLVISITOR_H
#define QPID_AMQP_0_10_CONTROLVISITOR_H
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

///
/// This file was automatically generated from the AMQP specification.
/// Do not edit.
///


#include "qpid/amqp_0_10/specification.h"

namespace qpid {
namespace amqp_0_10 {

struct ControlVisitor
{
    virtual ~ControlVisitor() {}
    typedef Control BaseType;
    virtual void visit(connection::Start&) = 0;
    virtual void visit(connection::StartOk&) = 0;
    virtual void visit(connection::Secure&) = 0;
    virtual void visit(connection::SecureOk&) = 0;
    virtual void visit(connection::Tune&) = 0;
    virtual void visit(connection::TuneOk&) = 0;
    virtual void visit(connection::Open&) = 0;
    virtual void visit(connection::OpenOk&) = 0;
    virtual void visit(connection::Redirect&) = 0;
    virtual void visit(connection::Heartbeat&) = 0;
    virtual void visit(connection::Close&) = 0;
    virtual void visit(connection::CloseOk&) = 0;
    virtual void visit(session::Attach&) = 0;
    virtual void visit(session::Attached&) = 0;
    virtual void visit(session::Detach&) = 0;
    virtual void visit(session::Detached&) = 0;
    virtual void visit(session::RequestTimeout&) = 0;
    virtual void visit(session::Timeout&) = 0;
    virtual void visit(session::CommandPoint&) = 0;
    virtual void visit(session::Expected&) = 0;
    virtual void visit(session::Confirmed&) = 0;
    virtual void visit(session::Completed&) = 0;
    virtual void visit(session::KnownCompleted&) = 0;
    virtual void visit(session::Flush&) = 0;
    virtual void visit(session::Gap&) = 0;
};
struct ConstControlVisitor
{
    virtual ~ConstControlVisitor() {}
    typedef const Control BaseType;
    virtual void visit(const connection::Start&) = 0;
    virtual void visit(const connection::StartOk&) = 0;
    virtual void visit(const connection::Secure&) = 0;
    virtual void visit(const connection::SecureOk&) = 0;
    virtual void visit(const connection::Tune&) = 0;
    virtual void visit(const connection::TuneOk&) = 0;
    virtual void visit(const connection::Open&) = 0;
    virtual void visit(const connection::OpenOk&) = 0;
    virtual void visit(const connection::Redirect&) = 0;
    virtual void visit(const connection::Heartbeat&) = 0;
    virtual void visit(const connection::Close&) = 0;
    virtual void visit(const connection::CloseOk&) = 0;
    virtual void visit(const session::Attach&) = 0;
    virtual void visit(const session::Attached&) = 0;
    virtual void visit(const session::Detach&) = 0;
    virtual void visit(const session::Detached&) = 0;
    virtual void visit(const session::RequestTimeout&) = 0;
    virtual void visit(const session::Timeout&) = 0;
    virtual void visit(const session::CommandPoint&) = 0;
    virtual void visit(const session::Expected&) = 0;
    virtual void visit(const session::Confirmed&) = 0;
    virtual void visit(const session::Completed&) = 0;
    virtual void visit(const session::KnownCompleted&) = 0;
    virtual void visit(const session::Flush&) = 0;
    virtual void visit(const session::Gap&) = 0;
};

}} // namespace qpid::amqp_0_10

#endif  /*!QPID_AMQP_0_10_CONTROLVISITOR_H*/
