	 / _____)             _              | |    
	( (____  _____ ____ _| |_ _____  ____| |__  
	 \____ \| ___ |    (_   _) ___ |/ ___)  _ \ 
	 _____) ) ____| | | || |_| ____( (___| | | |
	(______/|_____)_|_|_| \__)_____)\____)_| |_|
	  (C)2013 Semtech-Cycleo

Lora Gateway packet forwarder with GPS extensions
==================================================

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

Concentrator: radio RX/TX board, based on Semtech multichannel modems (SX130x), 
transceivers (SX135x) and/or low-power stand-alone modems (SX127x).

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

This program is statically linked with the libloragw "Lora Gateway HAL" library.
It was tested with v1.2.1 of the library but should work with any later 
version provided the API is v1 or a later backward-compatible API.
Data structures of the received packets are accessed by name (ie. not at a
binary level) so new functionalities can be added to the API without affecting
that program at all.

The last dependency is the hardware concentrator (based on FPGA or SX130x 
chips) that must be matched with the proper version of the HAL.

4. Usage
---------

To stop the application, press Ctrl+C.
Unless it's manually stopped or encounter a critical error, the program will 
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
