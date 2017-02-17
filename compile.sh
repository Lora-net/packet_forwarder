#!/bin/bash    

LORA_GATEWAY_DRIVER_PATH=../lora_gateway

TARGET_IP_ADDRESS=192.168.0.1
TARGET_PATH=/home/pi/lora-net
TARGET_USER=pi

clean_all() {
    make clean -C $LORA_GATEWAY_DRIVER_PATH
    if [ $? != 0 ]
    then
        echo "ERROR: Failed to clean $LORA_GATEWAY_DRIVER_PATH"
        exit 1
    fi
    make clean
    if [ $? != 0 ]
    then
        exit 1
    fi
}

build_all() {
    make all -C $LORA_GATEWAY_DRIVER_PATH
    if [ $? != 0 ]
    then
        echo "ERROR: Failed to compile $LORA_GATEWAY_DRIVER_PATH"
        exit 1
    fi
    make all
    if [ $? != 0 ]
    then
        exit 1
    fi
}

install() {
    scp ./lora_pkt_fwd/lora_pkt_fwd $TARGET_USER@$TARGET_IP_ADDRESS:$TARGET_PATH
    if [ $? != 0 ]
    then
        echo "ERROR: Failed to install the packet forwarder"
        echo "       target info: $TARGET_IP_ADDRESS, $TARGET_USER, $TARGET_PATH"
        exit 1
    fi
}   

case "$1" in
    install)
        install
    ;;

    clean)
        clean_all
    ;;

    cleanall)
        # clean and rebuild
        clean_all
        build_all
    ;;

    -h)
        echo "Compile the complete gateway software (driver & packet forwarder)"
        echo "Usage: $0 [clean/cleanall/install]"
        exit 1
    ;;

    *)
        # rebuild
        build_all
    ;;
esac

exit 0

