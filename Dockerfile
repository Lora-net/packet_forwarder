# Builds lora_pkt_fwd for each SPI bus and copies each to
# $OUTPUT_DIR/ respectively.

FROM balenalib/raspberry-pi-debian:buster-build as sx1301-builder

ENV ROOT_DIR=/opt
ENV LORA_GATEWAY_OUTPUT_DIR=/opt/output
ENV LORA_GATEWAY_INPUT_DIR="$ROOT_DIR/lora_gateway_builds"
ENV PACKET_FORWARDER_INPUT_DIR="$ROOT_DIR/packet_forwarder"

ENV OUTPUT_DIR="$ROOT_DIR/output"

WORKDIR "$ROOT_DIR"

# Copy upstream source into expected location
COPY . "$PACKET_FORWARDER_INPUT_DIR"
COPY --from=marvinnebra/lora_gateway "$LORA_GATEWAY_OUTPUT_DIR" "$LORA_GATEWAY_INPUT_DIR"

RUN . "$PACKET_FORWARDER_INPUT_DIR/compile_lora_pkt_fwd.sh"
