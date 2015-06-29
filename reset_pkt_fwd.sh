#!/bin/sh

# This script is intended to be used on IoT Starter Kit platform only, it performs the
# following actions:
#       - export/unpexort GPIO7 used to reset the SX1301 chip
#       - optionaly update the Gateway_ID field of given JSON configuration file, with MAC address
#
# Usage examples:
#       ./reset_pkt_fwd.sh start ./local_conf.json
#       ./gps_pkt_fwd
#       (stop with Ctrl-C)
#       ./reset_pkt_fwd.sh stop
#       ./reset_pkt_fwd.sh start
#       ./gps_pkt_fwd

# Force bypassing auto update of Gateway_ID in JSON conf file
IOT_SK_GWID_UPDATE=true

# The reset pin of SX1301 is wired with RPi GPIO7
IOT_SK_SX1301_RESET_PIN=7

WAIT_GPIO() {
    sleep 0.1
}

iot_sk_init() {
    # setup GPIO 7
    echo "$IOT_SK_SX1301_RESET_PIN" > /sys/class/gpio/export; WAIT_GPIO

    # set GPIO 7 as output
    echo "out" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/direction; WAIT_GPIO

    # write output for SX1301 reset
    echo "1" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/value; WAIT_GPIO
    echo "0" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/value; WAIT_GPIO

    # set GPIO 7 as input
    echo "in" > /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN/direction; WAIT_GPIO
}

iot_sk_term() {
    # cleanup GPIO 7
    if [ -d /sys/class/gpio/gpio$IOT_SK_SX1301_RESET_PIN ]
    then
        echo "$IOT_SK_SX1301_RESET_PIN" > /sys/class/gpio/unexport; WAIT_GPIO
    fi
}

iot_sk_update_gwid() {
    if [ -z "$1" ]
    then
        echo "Gateway_ID not set, using default"
    else
        # get gateway ID from its MAC address
        GWID_PREFIX="FFFE"
        GWID=$(ip link show eth0 | awk '/ether/ {print $2}' | awk -F\: '{print $1$2$3$4$5$6}')

        # replace last 8 digits of default gateway ID by actual GWID, in given JSON configuration file
        sed -i 's/\(^\s*"gateway_ID":\s*"\).\{16\}"\s*\(,\?\).*$/\1'${GWID_PREFIX}${GWID}'"\2/' $1

        echo "Gateway_ID set to "$GWID_PREFIX$GWID" in file "$1
    fi
}

case "$1" in
    start)
    iot_sk_term
    iot_sk_init
    if $IOT_SK_GWID_UPDATE; then
        iot_sk_update_gwid $2
    fi
    ;;
    stop)
    iot_sk_term
    ;;
    *)
    echo "Usage: $0 {start|stop} [filename]"
    echo "  filename:    Path to JSON file containing Gateway_ID for packet forwarder"
    exit 1
    ;;
esac

exit 0
