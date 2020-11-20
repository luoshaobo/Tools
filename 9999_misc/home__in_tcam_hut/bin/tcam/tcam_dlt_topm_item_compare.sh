#!/bin/bash

TOPM_FILE_PATH="$1"

CMDS=`cat <<EOF
MemAvailable

/usr/bin/voc
/usr/bin/coma-core
/usr/bin/assistancecaller
/usr/bin/vpom
/usr/bin/tcam-diag
/usr/bin/dlt-system
/lib/systemd/systemd-journald

/usr/bin/vehicle_comm
/usr/bin/IPCommandBroker
/usr/bin/volvo_conn_manager

/usr/bin/dlt-daemon
/usr/bin/netmgrd
/usr/bin/tcam-ua-vuc
/usr/bin/tcam-ua-config
/usr/bin/tcam-dc-ethernet
/usr/bin/thermal-mgr
/usr/bin/rtpaudiod
/usr/bin/tcam-ua-android
/usr/bin/spmd
/usr/bin/qti
/usr/bin/adbd
/usr/bin/atfwd_daemon
/usr/bin/qmuxd
/usr/bin/diagrebootapp
/usr/bin/reboot-daemon

/usr/sbin/wpa_supplicant
/usr/sbin/connmand
/usr/sbin/cgrulesengd
/usr/sbin/dnsmasq

/bin/dbus-daemon
/sbin/auditd
/sbin/init
/sbin/klogd

/lib/systemd/systemd-udevd
/lib/systemd/systemd-logind

/opt/conti/bin/conti-nadif
/opt/conti/bin/conti-mcm-svc
/opt/conti/bin/time-mgrd
/opt/conti/bin/location-mgrd
/opt/conti/bin/wifimgr
/opt/conti/bin/audioctrld
/opt/conti/bin/vcs
/opt/conti/bin/lifecycle-mgrd
/opt/conti/bin/qmi_cust_app_svc
/opt/conti/bin/per_update_agent
/opt/conti/bin/conti-mpmc
/opt/conti/bin/swl-mgrd
/opt/conti/bin/pasd
/opt/conti/bin/conti-psapcall
EOF
`

usage()
{
    echo "Usage:"
    echo "  `basename $0` <topm_file_path>"
}

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

if [ ! -r "$TOPM_FILE_PATH" ]; then
    echo "*** ERROR: file can't be read: $TOPM_FILE_PATH"
    exit 2
fi

for CMD in $CMDS; do
    if [ -z $CMD ]; then
        continue
    fi

    #echo "$CMD"
    cat "$TOPM_FILE_PATH" | grep "$CMD"
    echo ""
done
