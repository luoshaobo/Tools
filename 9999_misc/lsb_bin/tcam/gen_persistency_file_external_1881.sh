#!/bin/bash

NEW_PERSISTENCE_PACKAGE_PATH_NAME="$1"
OLD_PERSISTENCE_PACKAGE_PATH_NAME="$2"

function usage()
{
    echo "usage:" >&2
    echo "    `basename $0` [<new_persistence_package_path_name>] [<old_persistence_package_path_name>]" >&2
}

function main()
{
    if [ -z "$NEW_PERSISTENCE_PACKAGE_PATH_NAME" ]; then
        NEW_PERSISTENCE_PACKAGE_PATH_NAME="tp-pers-install.tar.gz.pc_1883"
    fi

    if [ -z "$OLD_PERSISTENCE_PACKAGE_PATH_NAME" ]; then
        OLD_PERSISTENCE_PACKAGE_PATH_NAME="tp-pers-install.tar.gz"
    fi

    echo "NEW_PERSISTENCE_PACKAGE_PATH_NAME: $NEW_PERSISTENCE_PACKAGE_PATH_NAME"
    echo "OLD_PERSISTENCE_PACKAGE_PATH_NAME: $OLD_PERSISTENCE_PACKAGE_PATH_NAME"

    if [ ! -r "$OLD_PERSISTENCE_PACKAGE_PATH_NAME" ]; then
        echo "*** ERROR: file can't be read: $OLD_PERSISTENCE_PACKAGE_PATH_NAME" >&2
        exit 1
    fi

    TMP_WORK_DIR=`mktemp -d`
    echo "TMP_WORK_DIR: $TMP_WORK_DIR"
    
    tar zxf "$OLD_PERSISTENCE_PACKAGE_PATH_NAME" -C "$TMP_WORK_DIR"
    
    mv -f "$TMP_WORK_DIR/persistence_config_fsm_ssm.xml" "$TMP_WORK_DIR/persistence_config_fsm_ssm.xml.orig"
    cat "$TMP_WORK_DIR/persistence_config_fsm_ssm.xml.orig" \
        | sed_in_scope.sh - "<name>signal_service_uri</name>" 10 "s/<default>.*<\/default>/<default>118.31.212.38:1881<\/default>/g" \
        | sed_in_scope.sh - "<name>signal_service_uri_ssl</name>" 10 "s/<default>.*<\/default>/<default>118.31.212.38:1881<\/default>/g" \
        > "$TMP_WORK_DIR/persistence_config_fsm_ssm.xml"

    mv -f "$TMP_WORK_DIR/persistence_config_remote.xml" "$TMP_WORK_DIR/persistence_config_remote.xml.orig"
    cat "$TMP_WORK_DIR/persistence_config_remote.xml.orig" \
        | sed_in_scope.sh - "<name>message-delivery_MQTT_SERVER_URL</name>" 10 "s/<default>.*<\/default>/<default>118.31.212.38:1881<\/default>/g" \
        > "$TMP_WORK_DIR/persistence_config_remote.xml"

    mv -f "$TMP_WORK_DIR/persistence_config_coma.xml" "$TMP_WORK_DIR/persistence_config_coma.xml.orig"
    cat "$TMP_WORK_DIR/persistence_config_coma.xml.orig" \
        | sed_in_scope.sh - "<name>tls_flag</name>" 10 "s/<default>.*<\/default>/<default>0<\/default>/g" \
        > "$TMP_WORK_DIR/persistence_config_coma.xml"

    (
        cd "$TMP_WORK_DIR"
        tar zcf /tmp/pp.tgz *
    )
    mv -f /tmp/pp.tgz "$NEW_PERSISTENCE_PACKAGE_PATH_NAME"

    rm -fr "$TMP_WORK_DIR"
}

main $*
