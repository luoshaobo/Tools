#!/bin/bash

INPUT_FILE="-"
OUTPUT_FILE="-"

APPS=`cat <<EOF
/usr/bin/reboot-daemon
/usr/bin/qti
/usr/bin/adbd
/usr/bin/tcam-diag
/usr/bin/dlt-system
/usr/bin/dlt-daemon
/usr/bin/qmuxd
/usr/bin/diagrebootapp
/usr/bin/vehicle_comm
/usr/bin/atfwd_daemon
/usr/bin/tcam-ua-android
/usr/bin/tcam-ua-vuc
/usr/bin/tcam-dc-ethernet
/usr/bin/tcam-ua-config
/usr/bin/netmgrd
/usr/bin/spmd
/usr/bin/coma-core
/usr/bin/thermal-mgr
/usr/bin/rtpaudiod
/usr/bin/volvo_conn_manager
/usr/bin/IPCommandBroker
/usr/bin/voc
/usr/bin/vpom
/usr/bin/assistancecaller
/usr/sbin/connmand
/usr/sbin/cgrulesengd
/usr/sbin/wpa_supplicant
/usr/sbin/dnsmasq
/opt/conti/bin/pasd
/opt/conti/bin/vcs
/opt/conti/bin/audioctrld
/opt/conti/bin/time-mgrd
/opt/conti/bin/conti-nadif
/opt/conti/bin/qmi_cust_app_svc
/opt/conti/bin/lifecycle-mgrd
/opt/conti/bin/conti-psapcall
/opt/conti/bin/swl-mgrd
/opt/conti/bin/conti-mpmc
/opt/conti/bin/per_update_agent
/opt/conti/bin/location-mgrd
/opt/conti/bin/conti-mcm-svc
/opt/conti/bin/wifimgr
EOF`

function usage
{
    echo "Usage:"
    echo "  `basename $0` [<INPUT_FILE>|-] [<OUTPUT_FILE>|-]"
    echo
}

if [ $# -eq 1 ]; then
    if [ "$1" == "--help" -o "$1" == "-H" ]; then
        usage
        exit 1
    fi
fi

if [ $# -ge 1 ]; then
    if [ ! -r "$1" ]; then
        echo "*** ERROR: can't read from $1" >&2
        exit 2
    else
        INPUT_FILE="$1"
    fi
fi

if [ $# -ge 2 ]; then
    if [ ! -w "$2" ]; then
        echo "*** ERROR: can't write to $2" >&2
        exit 2
    else
        OUTPUT_FILE="$2"
    fi
fi

if [ "$INPUT_FILE" == "-" ]; then
    INPUT_FILE="/dev/stdin"
fi

if [ "$OUTPUT_FILE" == "-" ]; then
    OUTPUT_FILE="/dev/stdout"
fi

INPUT_FILE_CONTENT=`cat "$INPUT_FILE"`

for APP in $APPS; do
    if [ -n "$APP" ]; then
        echo "$INPUT_FILE_CONTENT" | grep "$APP" > "$OUTPUT_FILE" 2>&1
        if [ $? -ne 0 ]; then
            echo "*** Not Found: $APP" > "$OUTPUT_FILE"
        fi
    fi
done




