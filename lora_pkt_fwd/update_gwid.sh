#!/bin/sh

# This script is a helper to update the Gateway_ID field of given
# JSON configuration file, as a EUI-64 address generated from the 48-bits MAC
# address of the device it is run from.
#
# Usage examples:
#       ./update_gwid.sh ./local_conf.json

_ether=""
get_default_interface() {
    netstat -rn | sed -nE '/^0.0.0.0/ s,^.* ([a-z]+[0-9])$,\1,p'
}

get_ether() {
    local iface
    iface="$1"
    ip link show ${iface} | sed -nE \
        's,^.*ether ([^[:space:]]+)[[:space:]].*$,\1,p'|tr '[a-z]' '[A-Z]'
}

get_ether_first3() {
    echo ${_ether}|cut -c 1-2,4-5,7-8
}

get_ether_last3() {
    echo ${_ether}|cut -c 10-11,13-14,16-17
}

iot_sk_update_gwid() {
    local iface conf _begin _end gwid
    iface=$(get_default_interface)
    conf="$1"
    if [ -z "${iface}" ]
    then
        echo "No IPv4 gateway configured. IPv6 currently unsupported." >&2
        return 78 # EX_CONFIG
    fi
    _ether=$(get_ether ${iface})
    if [ -z "${_ether}" ]
    then
        echo "Default interface does not have a MAC address." >&2
        return 78 # EX_CONFIG
    fi
    if [ ! -f ${conf} ]
    then
        echo "File ${conf} does not exist" >&2
        return 66 # EX_NOINPUT
    fi
    if [ ! -w ${conf} ]
    then
        echo "File ${conf} is not writeable by us" >&2
        return 73 # EX_CANTCREAT
    fi

    _begin=$(get_ether_first3)
    _end=$(get_ether_last3)
    gwid="${_begin}FFFE${_end}"


    # replace last 8 digits of default gateway ID by actual GWID, in given
    # JSON configuration file
    sed -i.bak -E \
        "s/(^[[:space:]]*\"gateway_ID\":[[:space:]]*\").{16}\"[[:space:]]*(,?).*$/\1${gwid}\"\2/" ${conf}

    echo "Gateway_ID set to ${gwid} in file ${conf}"
    echo "   To roll back: mv ${conf}.bak ${conf}"
    echo "   When satisfied: rm ${conf}.bak"
    return 0 # EX_OK
}

if [ $# -ne 1 ]
then
    echo "Usage: $0 <filename>" >&2
    echo "  filename: Path to JSON file containing Gateway_ID for packet forwarder" >&2
    exit 64 # EX_USAGE
fi 

iot_sk_update_gwid $1

exit $?
