#!/bin/sh

# This script is a helper to update the Gateway_ID field of given
# JSON configuration file, as a EUI-64 address generated from the 48-bits MAC
# address of the device it is run from.
#
# Usage examples:
#       ./update_gwid.sh ./local_conf.json

iot_sk_update_gwid() {
    # get gateway ID from its MAC address to generate an EUI-64 address
    GWID_MIDFIX="FFFE"
    GWID_BEGIN=$(ip link show eth0 | awk '/ether/ {print $2}' | awk -F\: '{print $1$2$3}')
    GWID_END=$(ip link show eth0 | awk '/ether/ {print $2}' | awk -F\: '{print $4$5$6}')

    # replace last 8 digits of default gateway ID by actual GWID, in given JSON configuration file
    sed -i 's/\(^\s*"gateway_ID":\s*"\).\{16\}"\s*\(,\?\).*$/\1'${GWID_BEGIN}${GWID_MIDFIX}${GWID_END}'"\2/' $1

    echo "Gateway_ID set to "$GWID_BEGIN$GWID_MIDFIX$GWID_END" in file "$1
}

if [ $# -ne 1 ]
then
    echo "Usage: $0 [filename]"
    echo "  filename: Path to JSON file containing Gateway_ID for packet forwarder"
    exit 1
fi 

iot_sk_update_gwid $1

exit 0
