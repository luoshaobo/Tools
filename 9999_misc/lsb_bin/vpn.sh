#!/bin/bash

export PATH=/cygdrive/c/"Program Files (x86)"/Cisco/"Cisco AnyConnect Secure Mobility Client":$PATH

VPNCLI="vpncli"
OP="$1"

function usage()
{
    echo "Usage: "
    echo "    `basename $0` <-|c|connect|d|disconnect|s|state>"
    echo ""
}

function check_connected()
{
    $VPNCLI state | grep "state: Connected" >/dev/null 2>&1
}

function main()
{
    if [ $# -le 0 ]; then
        usage
        exit 1
    fi

    if [ $OP = "-" ]; then
        shift
        $VPNCLI $*
    elif [ $OP = "connect" -o $OP = "c" ]; then
        check_connected
        if [ $? -ne 0 ]; then
            echo "=== Begin to connect ..."
            echo ""
            
            #
            # NOTE: the content in "d:/lsb/vpn_login_config.txt" looks like:
            #     connect "APAC - Shanghai_3"
            #     uidv4956@cw01.contiwan.com
            #     my_password
            #     exit
            #
            cat "d:/lsb/vpn_login_config.txt" | $VPNCLI -s
        else
            echo "=== Already connected!"
            echo ""
            $VPNCLI state
        fi
    elif [ $OP = "disconnect" -o $OP = "d"  ]; then
        check_connected
        if [ $? -eq 0 ]; then
            echo "=== Begin to disconnect ..."
            echo ""
            $VPNCLI disconnect
        else
            echo "=== Already disconnected!"
            echo ""
            $VPNCLI state
        fi
    elif [ $OP = "state" -o $OP = "s"  ]; then
        $VPNCLI state
    else
        $VPNCLI $*
    fi
}

main $*
