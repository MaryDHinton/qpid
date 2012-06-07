/* Licensed to the Apache Software Foundation (ASF) under one
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
package org.apache.qpid.messaging.cpp;

import org.apache.qpid.messaging.Connection;
import org.apache.qpid.messaging.ConnectionFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CppConnectionFactory extends ConnectionFactory
{
    private static final Logger _logger = LoggerFactory.getLogger(CppConnectionFactory.class);
    
    static 
    {
        System.loadLibrary("cqpid_java");
        _logger.info("native qpid library was loaded sucessfully");
    }
    
    public CppConnectionFactory()
    {        
    }

    public Connection create(String url)
    {
        return new CppConnection(url);
    }

    public Connection createConnection(String url)
    {
        return create(url);
    }
}
