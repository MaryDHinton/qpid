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
package org.apache.qpid.server.model.adapter;

import java.util.Map;
import java.util.Set;
import java.util.UUID;

import org.apache.qpid.server.configuration.IllegalConfigurationException;
import org.apache.qpid.server.model.Broker;
import org.apache.qpid.server.model.Port;
import org.apache.qpid.server.model.Protocol;
import org.apache.qpid.server.registry.IApplicationRegistry;
import org.apache.qpid.server.transport.AmqpPortAdapter;
import org.apache.qpid.server.util.MapValueConverter;

public class PortFactory
{
    private final IApplicationRegistry _applicationRegistry;
    private final PortAttributeDestringifier _portAttributeDestringifier = new PortAttributeDestringifier();

    public PortFactory(IApplicationRegistry applicationRegistry)
    {
        _applicationRegistry = applicationRegistry;
    }

    /**
     * @param attributes the port attributes, where the values are strings, either singly or in collections.
     */
    public Port createPort(UUID id, Broker broker, Map<String, Object> attributesAsStrings)
    {
        Map<String, Object> attributes = _portAttributeDestringifier.destringify(attributesAsStrings);
        final Port port;
        if (isAmqpProtocol(attributes))
        {
            port = new AmqpPortAdapter(id, broker, attributes, _applicationRegistry);
        }
        else
        {
            port = new PortAdapter(id, broker, attributes);
        }
        return port;
    }

    private boolean isAmqpProtocol(Map<String, Object> portAttributes)
    {
        Set<Object> protocolStrings = MapValueConverter.getSetAttribute(Port.PROTOCOLS, portAttributes);
        int amqpProtocolsFound = 0;
        int nonAmqpProtocolsFound = 0;
        for (Object protocolObject : protocolStrings)
        {
            Protocol protocol = Protocol.valueOfObject(protocolObject);
            if (protocol.isAMQP())
            {
                amqpProtocolsFound++;
            }
            else
            {
                nonAmqpProtocolsFound++;
            }
        }

        if (amqpProtocolsFound > 0)
        {
            if (nonAmqpProtocolsFound > 0)
            {
                throw new IllegalConfigurationException("Found AMQP and non-AMQP protocols on port configuration: "
                        + portAttributes);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
}