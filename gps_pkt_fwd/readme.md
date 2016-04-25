	 / _____)             _              | |    
	( (____  _____ ____ _| |_ _____  ____| |__  
	 \____ \| ___ |    (_   _) ___ |/ ___)  _ \ 
	 _____) ) ____| | | || |_| ____( (___| | | |
	(______/|_____)_|_|_| \__)_____)\____)_| |_|
	  (C)2013 Semtech-Cycleo

Lora Gateway packet forwarder with GPS extensions
=================================================

1. Introduction
----------------

The GPS packet forwarder is a program running on the host of a Lora Gateway 
that forward RF packets receive by the concentrator to a server through a 
IP/UDP link, and emits RF packets that are sent by the server.

To learn more about the network protocol between the gateway and the server, 
please read the PROTOCOL.TXT document.

2. System schematic and definitions
------------------------------------

	((( Y )))
	    |
	    |
	+- -|- - - - - - - - - - - - -+        xxxxxxxxxxxx          +--------+
	|+--+-----------+     +------+|       xx x  x     xxx        |        |
	||              |     |      ||      xx  Internet  xx        |        |
	|| Concentrator |<----+ Host |<------xx     or    xx-------->|        |
	||              | SPI |      ||      xx  Intranet  xx        | Server |
	|+--------------+     +------+|       xxxx   x   xxxx        |        |
	|   ^                    ^    |           xxxxxxxx           |        |
	|   | PPS  +-----+  NMEA |    |                              |        |
	|   +------| GPS |-------+    |                              +--------+
	|          +-----+            |
	|                             |
	|            Gateway          |
	+- - - - - - - - - - - - - - -+

Concentrator: radio RX/TX board, based on Semtech multichannel modems (SX1301),
transceivers (SX125x) and/or low-power stand-alone modems (SX127x).

Host: embedded computer on which the packet forwarder is run. Drives the 
concentrator through a SPI link.

Gateway: a device composed of at least one radio concentrator, a host, some 
network connection to the internet or a private network (Ethernet, 3G, Wifi, 
microwave link), and optionally a GPS receiver for synchronization. 

Server: an abstract computer that will process the RF packets received and 
forwarded by the gateway, and issue RF packets in response that the gateway 
will have to emit.


3. Dependencies
----------------

This program uses the Parson library (http://kgabis.github.com/parson/) by
Krzysztof Gabis for JSON parsing.
Many thanks to him for that very practical and well written library.

This program is statically linked with the libloragw Lora concentrator library.
Data structures of the received packets are accessed by name (ie. not at a
binary level) so new functionalities can be added to the API without affecting
that program at all.

This program follows the v1.1 version of the gateway-to-server protocol.

The last dependency is the hardware concentrator (based on SX1301 chips) that
must be matched with the proper version of the HAL.

4. Usage
---------

1. Update JSON configuration files, as explained below.
2. For IoT Starter Kit only, run:
    ./reset_pkd_fwd.sh stop
    ./reset_pkd_fwd.sh start local_conf.json
3. Run:
    ./gps_pkt_fwd

To stop the application, press Ctrl+C.
Unless it is manually stopped or encounter a critical error, the program will 
run forever.

There are no command line launch options.

The way the program takes configuration files into account is the following:
 * if there is a debug_conf.json parse it, others are ignored
 * if there is a global_conf.json parse it, look for the next file
 * if there is a local_conf.json parse it
If some parameters are defined in both global and local configuration files, 
the local definition overwrites the global definition. 

The global configuration file should be exactly the same throughout your 
network, contain all global parameters (parameters for "sensor" radio 
channels) and preferably default "safe" values for parameters that are 
specific for each gateway (eg. specify a default MAC address).
As some of the parameters (like 'rssi_offset', 'tx_lut_*') are board dependant,
several flavours of the global_conf.json file are provided in the cfg/
directory.
    * global_conf.json.PCB_E286.EU868: to be used for Semtech reference design
        board with PCB name PCB_E286 (also called Gateway Board v1.0 (no FPGA)).
        Configured for Europe 868MHz channels.
    * global_conf.json.PCB_E336.EU868: to be used for Semtech reference design
        board with PCB name PCB_E336 (also called Gateway Board v1.5 (with FPGA)).
        Configured for Europe 868MHz channels.
    * global_conf.json.US902: to be used for Semtech reference design v1.0 or
        v1.5. (No calibration done for RSSI offset and TX gains yet).
        Configured for US 902MHz channels.
Rename the one you need to global_conf.json before launching the packet
forwarder.

The local configuration file should contain parameters that are specific to 
each gateway (eg. MAC address, frequency for backhaul radio channels).

In each configuration file, the program looks for a JSON object named 
"SX1301_conf" that should contain the parameters for the Lora concentrator 
board (RF channels definition, modem parameters, etc) and another JSON object 
called "gateway_conf" that should contain the gateway parameters (gateway MAC 
address, IP address of the server, keep-alive time, etc).

To learn more about the JSON configuration format, read the provided JSON 
files and the libloragw API documentation.

Every X seconds (parameter settable in the configuration files) the program 
display statistics on the RF packets received and sent, and the network 
datagrams received and sent.
The program also send some statistics to the server in JSON format.

5. License
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

6. License for Parson library
------------------------------

Parson ( http://kgabis.github.com/parson/ )
Copyright (C) 2012 Krzysztof Gabis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*EOF*
