#!/usr/bin/env sh

compile_lora_pkt_fwd_for_spi_bus() {
    spi_bus="$1"
    echo "Compiling upstream lora_pkt_fwd for sx1301 on $spi_bus"

    # lora_pkt_fwd Makefile expects lora_gateway to be to levels up
    rm -rf "$ROOT_DIR/lora_gateway"
    mkdir -p "$ROOT_DIR/lora_gateway"
    cp -r "$LORA_GATEWAY_INPUT_DIR/$spi_bus/" "$ROOT_DIR/lora_gateway/libloragw/"

    cd "$PACKET_FORWARDER_INPUT_DIR/lora_pkt_fwd" || exit
    make clean
    make -j 4

    cp -R "$PACKET_FORWARDER_INPUT_DIR/lora_pkt_fwd/lora_pkt_fwd" "$OUTPUT_DIR/lora_pkt_fwd_$spi_bus"

    echo "Finished building lora_pkt_fwd for sx1301 on $spi_bus in $OUTPUT_DIR"
}

compile_lora_pkt_fwd() {
    echo "Compiling libloragw for sx1301 concentrator on all the necessary SPI buses in $ROOT_DIR"
    
    # Built outputs will be copied to this directory
    mkdir -p "$OUTPUT_DIR"
    
    # In order to be more portable, intentionally not interating over an array
    compile_lora_pkt_fwd_for_spi_bus spidev0.0
    compile_lora_pkt_fwd_for_spi_bus spidev0.1
    compile_lora_pkt_fwd_for_spi_bus spidev1.0
    compile_lora_pkt_fwd_for_spi_bus spidev1.1
    compile_lora_pkt_fwd_for_spi_bus spidev1.2
    compile_lora_pkt_fwd_for_spi_bus spidev2.0
    compile_lora_pkt_fwd_for_spi_bus spidev2.1
    compile_lora_pkt_fwd_for_spi_bus spidev32766.0
}

compile_lora_pkt_fwd

