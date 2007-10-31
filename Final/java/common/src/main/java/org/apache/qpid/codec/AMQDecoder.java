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
package org.apache.qpid.codec;

import org.apache.mina.common.ByteBuffer;
import org.apache.mina.common.IoSession;
import org.apache.mina.filter.codec.CumulativeProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;

import org.apache.qpid.framing.AMQDataBlockDecoder;
import org.apache.qpid.framing.ProtocolInitiation;

/**
 * AMQDecoder delegates the decoding of AMQP either to a data block decoder, or in the case of new connections, to a
 * protocol initiation decoder. It is a cumulative decoder, which means that it can accumulate data to decode in the
 * buffer until there is enough data to decode.
 *
 * <p/>One instance of this class is created per session, so any changes or configuration done at run time to the
 * decoder will only affect decoding of the protocol session data to which is it bound.
 *
 * <p/><table id="crc"><caption>CRC Card</caption>
 * <tr><th> Responsibilities <th> Collaborations
 * <tr><td> Delegate protocol initiation to its decoder. <td> {@link ProtocolInitiation.Decoder}
 * <tr><td> Delegate AMQP data to its decoder. <td> {@link AMQDataBlockDecoder}
 * <tr><td> Accept notification that protocol initiation has completed.
 * </table>
 *
 * @todo If protocol initiation decoder not needed, then don't create it. Probably not a big deal, but it adds to the
 *       per-session overhead.
 */
public class AMQDecoder extends CumulativeProtocolDecoder
{
    /** Holds the 'normal' AMQP data decoder. */
    private AMQDataBlockDecoder _dataBlockDecoder = new AMQDataBlockDecoder();

    /** Holds the protocol initiation decoder. */
    private ProtocolInitiation.Decoder _piDecoder = new ProtocolInitiation.Decoder();

    /** Flag to indicate whether this decoder needs to handle protocol initiation. */
    private boolean _expectProtocolInitiation;

    /**
     * Creates a new AMQP decoder.
     *
     * @param expectProtocolInitiation <tt>true</tt> if this decoder needs to handle protocol initiation.
     */
    public AMQDecoder(boolean expectProtocolInitiation)
    {
        _expectProtocolInitiation = expectProtocolInitiation;
    }

    /**
     * Delegates decoding AMQP from the data buffer that Mina has retrieved from the wire, to the data or protocol
     * intiation decoders.
     *
     * @param session The Mina session.
     * @param in      The raw byte buffer.
     * @param out     The Mina object output gatherer to write decoded objects to.
     *
     * @return <tt>true</tt> if the data was decoded, <tt>false<tt> if more is needed and the data should accumulate.
     *
     * @throws Exception If the data cannot be decoded for any reason.
     */
    protected boolean doDecode(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception
    {
        if (_expectProtocolInitiation)
        {
            return doDecodePI(session, in, out);
        }
        else
        {
            return doDecodeDataBlock(session, in, out);
        }
    }

    /**
     * Decodes AMQP data, delegating the decoding to an {@link AMQDataBlockDecoder}.
     *
     * @param session The Mina session.
     * @param in      The raw byte buffer.
     * @param out     The Mina object output gatherer to write decoded objects to.
     *
     * @return <tt>true</tt> if the data was decoded, <tt>false<tt> if more is needed and the data should accumulate.
     *
     * @throws Exception If the data cannot be decoded for any reason.
     */
    protected boolean doDecodeDataBlock(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception
    {
        int pos = in.position();
        boolean enoughData = _dataBlockDecoder.decodable(session, in);
        in.position(pos);
        if (!enoughData)
        {
            // returning false means it will leave the contents in the buffer and
            // call us again when more data has been read
            return false;
        }
        else
        {
            _dataBlockDecoder.decode(session, in, out);

            return true;
        }
    }

    /**
     * Decodes an AMQP initiation, delegating the decoding to a {@link ProtocolInitiation.Decoder}.
     *
     * @param session The Mina session.
     * @param in      The raw byte buffer.
     * @param out     The Mina object output gatherer to write decoded objects to.
     *
     * @return <tt>true</tt> if the data was decoded, <tt>false<tt> if more is needed and the data should accumulate.
     *
     * @throws Exception If the data cannot be decoded for any reason.
     */
    private boolean doDecodePI(IoSession session, ByteBuffer in, ProtocolDecoderOutput out) throws Exception
    {
        boolean enoughData = _piDecoder.decodable(session, in);
        if (!enoughData)
        {
            // returning false means it will leave the contents in the buffer and
            // call us again when more data has been read
            return false;
        }
        else
        {
            _piDecoder.decode(session, in, out);

            return true;
        }
    }

    /**
     * Sets the protocol initation flag, that determines whether decoding is handled by the data decoder of the protocol
     * initation decoder. This method is expected to be called with <tt>false</tt> once protocol initation completes.
     *
     * @param expectProtocolInitiation <tt>true</tt> to use the protocol initiation decoder, <tt>false</tt> to use the
     *                                data decoder.
     */
    public void setExpectProtocolInitiation(boolean expectProtocolInitiation)
    {
        _expectProtocolInitiation = expectProtocolInitiation;
    }
}
