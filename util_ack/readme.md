	 / _____)             _              | |    
	( (____  _____ ____ _| |_ _____  ____| |__  
	 \____ \| ___ |    (_   _) ___ |/ ___)  _ \ 
	 _____) ) ____| | | || |_| ____( (___| | | |
	(______/|_____)_|_|_| \__)_____)\____)_| |_|
	  (C)2013 Semtech-Cycleo

Utility: packet acknowledger
=============================

1. Introduction
----------------

The packet acknowledger is a simple helper program listening on a single UDP 
port and responding to PUSH_DATA datagrams with PUSH_ACK, and to PULL_DATA 
datagrams with PULL_ACK.

Informations about the datagrams received and the answers send are display on 
screen to help communication debugging.

Packets not following the protocol detailed in the PROTOCOL.TXT document in the
basic_pkt_fwt directory are ignored.

2. Dependencies
----------------

This program follows the v1.0 version of the gateway-to-server protocol.

3. Usage
---------

Start the program with the port number as first and only argument.

To stop the application, press Ctrl+C.
