#!/usr/bin/env perl
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
use strict;
use warnings;

use qpid;

my $url = ( @ARGV == 1 ) ? $ARGV[0] : "amqp:tcp:127.0.0.1:5672";
my $connectionOptions =  ( @ARGV > 1 ) ? $ARGV[1] : "";


my $connection = new qpid::messaging::Connection($url, $connectionOptions);

eval {
$connection->open();
my $session = $connection->create_session();

my $sender = $session->create_sender("service_queue");

#create temp queue & receiver...
my $responseQueue = new qpid::messaging::Address("#response-queue; {create:always, delete:always}");
my $receiver = $session->create_receiver($responseQueue);

#Now send some messages...

my @s = (
      "Twas brillig, and the slithy toves",
      "Did gire and gymble in the wabe.",
      "All mimsy were the borogroves,",
      "And the mome raths outgrabe."
     );

my $request = new qpid::messaging::Message();
$request->set_reply_to($responseQueue);
for (my $i=0; $i<4; $i++) {
    $request->set_content($s[$i]);
    $sender->send($request);
    my $response = $receiver->fetch();
    print $request->get_content() . " -> " . $response->get_content() . "\n";
}

$connection->close();
};

if ($@) {
    die $@;
}


