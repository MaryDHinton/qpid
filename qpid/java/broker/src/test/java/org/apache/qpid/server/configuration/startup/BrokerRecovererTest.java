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
package org.apache.qpid.server.configuration.startup;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.same;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.UUID;

import junit.framework.TestCase;

import org.apache.qpid.server.configuration.ConfigurationEntry;
import org.apache.qpid.server.configuration.ConfiguredObjectRecoverer;
import org.apache.qpid.server.configuration.RecovererProvider;
import org.apache.qpid.server.model.AuthenticationProvider;
import org.apache.qpid.server.model.Broker;
import org.apache.qpid.server.model.ConfiguredObject;
import org.apache.qpid.server.model.ConfiguredObjectType;
import org.apache.qpid.server.model.GroupProvider;
import org.apache.qpid.server.model.Port;
import org.apache.qpid.server.model.VirtualHost;
import org.apache.qpid.server.model.adapter.AuthenticationProviderFactory;
import org.apache.qpid.server.model.adapter.PortFactory;
import org.apache.qpid.server.registry.IApplicationRegistry;

public class BrokerRecovererTest extends TestCase
{
    private BrokerRecoverer _brokerRecoverer;

    private IApplicationRegistry _applicationRegistry = mock(IApplicationRegistry.class);

    private PortRecoverer _portRecoverer = mock(PortRecoverer.class);
    private AuthenticationProviderRecoverer _authenticationProviderRecoverer = mock(AuthenticationProviderRecoverer.class);

    private AuthenticationProviderFactory _authenticationProviderFactory = mock(AuthenticationProviderFactory.class);
    private PortFactory _portFactory = mock(PortFactory.class);

    private GroupProviderRecoverer _groupProviderRecoverer = mock(GroupProviderRecoverer.class);

    private ConfigurationEntry _entry = mock(ConfigurationEntry.class);

    private UUID _brokerId = UUID.randomUUID();
    private Map<ConfiguredObjectType, Collection<ConfigurationEntry>> _entryChildren = new HashMap<ConfiguredObjectType, Collection<ConfigurationEntry>>();

    private PluginRecoverer _pluginRecoverer = mock(PluginRecoverer.class);


    @Override
    protected void setUp() throws Exception
    {
        super.setUp();
        _brokerRecoverer = new BrokerRecoverer(_authenticationProviderFactory, _portFactory, _applicationRegistry);
        when(_entry.getId()).thenReturn(_brokerId);
        when(_entry.getChildren()).thenReturn(_entryChildren);

    }

    public void testCreateBrokerWithVirtualHost()
    {
        final ConfigurationEntry virtualHostEntry = mock(ConfigurationEntry.class);
        when(virtualHostEntry.getType()).thenReturn(ConfiguredObjectType.VIRTUAL_HOST);
        _entryChildren.put(ConfiguredObjectType.VIRTUAL_HOST, Arrays.asList(virtualHostEntry));

        final VirtualHost virtualHost = mock(VirtualHost.class);

        RecovererProvider recovererProvider = createRecoveryProvider(virtualHostEntry, virtualHost);

        Broker broker = _brokerRecoverer.create(recovererProvider, _entry);

        assertNotNull(broker);
        assertEquals(_brokerId, broker.getId());
        assertEquals(1, broker.getVirtualHosts().size());
        assertEquals(virtualHost, broker.getVirtualHosts().iterator().next());

        // XXX test top-level attributes eg status-updates
    }

    public void testCreateBrokerWithPorts()
    {
        ConfigurationEntry portEntry = mock(ConfigurationEntry.class);
        Port port = mock(Port.class);
        _entryChildren.put(ConfiguredObjectType.PORT, Arrays.asList(portEntry));

        RecovererProvider recovererProvider = createRecoveryProvider(portEntry, port);
        when(_portRecoverer.create(same(recovererProvider), same(portEntry), any(Broker.class))).thenReturn(port);

        Broker broker = _brokerRecoverer.create(recovererProvider, _entry);

        assertNotNull(broker);
        assertEquals(_brokerId, broker.getId());
        assertEquals(Collections.singletonList(port), broker.getPorts());
    }

    public void testCreateBrokerWithAuthenticationProviders()
    {
        ConfigurationEntry authenticationProviderEntry = mock(ConfigurationEntry.class);
        AuthenticationProvider authenticationProvider = mock(AuthenticationProvider.class);
        _entryChildren.put(ConfiguredObjectType.AUTHENTICATION_PROVIDER, Arrays.asList(authenticationProviderEntry));


        RecovererProvider recovererProvider = createRecoveryProvider(authenticationProviderEntry, authenticationProvider);

        when(_authenticationProviderRecoverer.create(same(recovererProvider), same(authenticationProviderEntry),
                any(Broker.class))).thenReturn(authenticationProvider);
        Broker broker = _brokerRecoverer.create(recovererProvider, _entry);

        assertNotNull(broker);
        assertEquals(_brokerId, broker.getId());
        assertEquals(Collections.singletonList(authenticationProvider), broker.getAuthenticationProviders());
    }

    public void testCreateBrokerWithGroupProviders()
    {
        ConfigurationEntry groupProviderEntry = mock(ConfigurationEntry.class);
        GroupProvider groupProvider = mock(GroupProvider.class);
        _entryChildren.put(ConfiguredObjectType.GROUP_PROVIDER, Arrays.asList(groupProviderEntry));

        RecovererProvider recovererProvider = createRecoveryProvider(groupProviderEntry, groupProvider);
        when(_groupProviderRecoverer.create(same(recovererProvider), same(groupProviderEntry), any(Broker.class))).thenReturn(groupProvider);

        Broker broker = _brokerRecoverer.create(recovererProvider, _entry);

        assertNotNull(broker);
        assertEquals(_brokerId, broker.getId());
        assertEquals(Collections.singletonList(groupProvider), broker.getGroupProviders());
    }

    public void testCreateBrokerWithPlugins()
    {
        ConfigurationEntry pluginEntry = mock(ConfigurationEntry.class);
        ConfiguredObject plugin = mock(ConfiguredObject.class);
        _entryChildren.put(ConfiguredObjectType.PLUGIN, Arrays.asList(pluginEntry));

        RecovererProvider recovererProvider = createRecoveryProvider(pluginEntry, plugin);
        when(_pluginRecoverer.create(same(recovererProvider), same(pluginEntry), any(Broker.class))).thenReturn(plugin);

        Broker broker = _brokerRecoverer.create(recovererProvider, _entry);

        assertNotNull(broker);
        assertEquals(_brokerId, broker.getId());
        assertEquals(Collections.singleton(plugin), new HashSet<ConfiguredObject>(broker.getChildren(ConfiguredObject.class)));
    }

    private <T extends ConfiguredObject> RecovererProvider createRecoveryProvider(final ConfigurationEntry entry, final T objectToRecoverer)
    {
        RecovererProvider recovererProvider = new RecovererProvider()
        {
            @Override
            public ConfiguredObjectRecoverer<? extends ConfiguredObject> getRecoverer(ConfiguredObjectType type)
            {
                @SuppressWarnings("unchecked")
                final ConfiguredObjectRecoverer<T> recovever = mock(ConfiguredObjectRecoverer.class);
                when(recovever.create(same(this), same(entry), any(Broker.class))).thenReturn(objectToRecoverer);
                return recovever;
            }
        };
        return recovererProvider;
    }
}