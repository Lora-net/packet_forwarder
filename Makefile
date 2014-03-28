### Environment constants 

LGW_PATH := ../../lora_gateway/libloragw
CROSS_COMPILE :=
export

### general build targets

all:
	$(MAKE) all -e -C basic_pkt_fwd
	$(MAKE) all -e -C gps_pkt_fwd
	$(MAKE) all -e -C beacon_pkt_fwd
	$(MAKE) all -e -C util_ack
	$(MAKE) all -e -C util_sink
	$(MAKE) all -e -C util_tx_test

clean:
	$(MAKE) clean -e -C basic_pkt_fwd
	$(MAKE) clean -e -C gps_pkt_fwd
	$(MAKE) clean -e -C beacon_pkt_fwd
	$(MAKE) clean -e -C util_ack
	$(MAKE) clean -e -C util_sink
	$(MAKE) clean -e -C util_tx_test

### EOF