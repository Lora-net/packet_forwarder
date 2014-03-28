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

Use the -n option to specify on which UDP port the program must wait for a 
gateway to contact it.

Use the -f option followed by a real number (decimal point and scientific
'E notation' are OK) to specify the modulation central frequency.

Use the -s option to specify the Spreading Factor of Lora modulation (values 7
to 12 are valid).

Use the -b option to set Lora modulation bandwidth in kHz (accepted values: 125,
250 or 500).

Use the -p option to set the concentrator TX power in dBm. Not all values are
supported by hardware (typically 14 et 20 dBm are supported, other values might
not give expected power). Check with a RF power meter before connecting any
sensitive equipment.

Use the -t option to specify the number of milliseconds of pause between
packets. Using zero will result in a quasi-continuous emission.

Use the -x to specify how many packets should be sent.

Use the -i option to invert the Lora modulation polarity.

The packets are 20 bytes long, and protected by the smallest supported ECC.

The payload content is:
[T][E][S][T][10's decimal digit of the packet counter in ASCII][unit decimal 
digit of the packet counter in ASCII] followed by ASCII padding.

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