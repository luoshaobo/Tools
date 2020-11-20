#!/bin/bash

VERSION="$1"

function usage()
{
    echo "Usage:"
    echo "  `basename $0` <VERSION>"
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit
fi

MAKE_PATH_IN_ADK=`(ls $HOME/SyDK_${VERSION}_*/NAVI_NAC_CN/imx6x-std-adk/x86_64-linux/usr/bin/make) 2>/dev/null`
if [ $? -ne 0 ]; then
    echo "*** Error: 'make' is not found in ADK!" >&2
    exit
fi

mv ${MAKE_PATH_IN_ADK} ${MAKE_PATH_IN_ADK}.001
echo "${MAKE_PATH_IN_ADK} is moved to ${MAKE_PATH_IN_ADK}.001"
