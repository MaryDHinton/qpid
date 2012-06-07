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

import java.util.Map;

public class TextMessage implements org.apache.qpid.messaging.Message
{

    org.apache.qpid.messaging.cpp.jni.Message _cppMessage;
    
    public TextMessage(String text)
    {
        _cppMessage = new org.apache.qpid.messaging.cpp.jni.Message(text);
        _cppMessage.setContentType("text/plain");
    }
    
    @Override
    public Object getContent()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public String getMessageId()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setMessageId(String messageId)
    {
        _cppMessage.setMessageId(messageId);

    }

    @Override
    public String getSubject()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setSubject(String subject)
    {
        _cppMessage.setMessageId(subject);
    }

    @Override
    public String getContentType()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setContentType(String contentType)
    {
        _cppMessage.setContentType(contentType);
    }

    @Override
    public String getCorrelationId()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setCorrelationId(String correlationId)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public String getReplyTo()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setReplyTo(String replyTo)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public String getUserId()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setUserId(String userId)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public boolean isDurable()
    {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public void setDurable(boolean durable)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public boolean isRedelivered()
    {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public void setRedelivered(boolean redelivered)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public int getPriority()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void setPriority(int priority)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public long getTtl()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void setTtl(long ttl)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public long getTimestamp()
    {
        // TODO Auto-generated method stub
        return 0;
    }

    @Override
    public void setTimestamp(long timestamp)
    {
        // TODO Auto-generated method stub

    }

    @Override
    public Map<String, Object> getProperties()
    {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void setProperties(Map<String, Object> properties)
    {
        // TODO Auto-generated method stub
    }
    
    protected org.apache.qpid.messaging.cpp.jni.Message getCppMessage()
    {
        return _cppMessage;
    }

}
