	 / _____)             _              | |    
	( (____  _____ ____ _| |_ _____  ____| |__  
	 \____ \| ___ |    (_   _) ___ |/ ___)  _ \ 
	 _____) ) ____| | | || |_| ____( (___| | | |
	(______/|_____)_|_|_| \__)_____)\____)_| |_|
	  (C)2013 Semtech-Cycleo

Utility: network packet sender
===============================

1. Introduction
----------------

The network packet sender is a simple helper program used to send packets 
through the gateway-to-server downlink route.

The program start by waiting for a gateway to send it a PULL_DATA datagram.
After that, it will send back to the gateway a specified amount of PULL_RESP 
datagrams, each containing a packet to be sent immediately and a variable 
payload.

2. Dependencies
----------------

This program follows the v1.1 version of the gateway-to-server protocol.

3. Usage
---------

The application runs until the specified number of packets have been sent.
Press Ctrl+C to stop the application before that.

Use the -h option to get help and details about available options.

The packets are [9-n] bytes long, and have following payload content:
```
+----------+---------------+---------------+---------------+---------------+---+---+---+---+---+---+---+---+
|    Id    | PktCnt[31:24] | PktCnt[23:16] | PktCnt[15:8]  | PktCnt[7:0]   | P | E | R |FCS| 0 | 1 |...| n |
+----------+---------------+---------------+---------------+---------------+---+---+---+---+---+---+---+---+

Id            : User defined ID to differentiate sender at receiver side. (8 bits)
PktCnt        : Packet counter incremented at each transmission. (32 bits)
‘P’, ‘E’, ‘R’ : ASCII values for characters 'P', 'E' and 'R'.
FCS           : Checksum: 8-bits sum of Id, PktCnt[31 :24] , PktCnt[23 :16] , PktCnt[15 :8] , PktCnt[7:0], ‘P’,’E’,’R’
0,1, ..., n   : Padding bytes up until user specified payload length.
```

4. License
-----------

Copyright (C) 2013, SEMTECH S.A.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
* Neither the name of the Semtech corporation nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL SEMTECH S.A. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*EOF*
