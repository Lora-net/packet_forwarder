# Builds lora_pkt_fwd for each SPI bus and copies each to
# $OUTPUT_DIR/ respectively.

FROM balenalib/raspberry-pi-debian:buster-build as lora-pkt-fwd-sx1301-builder

ENV ROOT_DIR=/opt

# Build output of nebraltd/lora_gateway
ENV LORA_GATEWAY_OUTPUT_DIR=/opt/output

# Intermediary location files from LORA_GATEWAY_OUTPUT_DIR are copied to
ENV LORA_GATEWAY_INPUT_DIR="$ROOT_DIR/lora_gateway_builds"

# Location source files for nebraltd/packet_forwarder are copied to
ENV PACKET_FORWARDER_INPUT_DIR="$ROOT_DIR/packet_forwarder"

# Output built files to this location
ENV OUTPUT_DIR="$ROOT_DIR/output"

WORKDIR "$ROOT_DIR"

# Copy files into expected location
COPY . "$PACKET_FORWARDER_INPUT_DIR"
COPY --from=nebraltd/lora_gateway:9c4b1d0c79645c3065aa4c2f3019c14da6cb2675 "$LORA_GATEWAY_OUTPUT_DIR" "$LORA_GATEWAY_INPUT_DIR"

# Compile lora_pkt_fwd for all buses
RUN . "$PACKET_FORWARDER_INPUT_DIR/compile_lora_pkt_fwd.sh"
