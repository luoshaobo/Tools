#!/bin/bash

ADK_VERSION="$1"

function usage()
{
    echo "usage:"
    echo "  `basename $0` <adk_version>"
    echo "    <adk_version>: such as \"17.11.04.01\""
    echo ""
}

if [ $# -lt 1 ]; then
    usage
    exit 0
fi

ENV_SH_PATH=`ls ~/SyDK_${ADK_VERSION}_*/NAC_CN/imx6x-std-adk/bin/env.sh`
ENV_SH_BASEDIR=`dirname "$ENV_SH_PATH"`

if [ ! -d "$ENV_SH_BASEDIR" ]; then
    echo "*** Error: \"$ENV_SH_BASEDIR\" is not a valid dir!" >&2
    exit 1
fi

if [ ! -f "$ENV_SH_BASEDIR/env.sh.orig" ]; then
    if [ ! -f "$ENV_SH_BASEDIR/env.sh" ]; then
        echo "*** Error: there is no env.sh in dir \"$ENV_SH_BASEDIR\"!" >&2
        exit 1
    fi
    mv -f "$ENV_SH_BASEDIR/env.sh" "$ENV_SH_BASEDIR/env.sh.orig"
fi

if [ ! -f "$ENV_SH_BASEDIR/env.sh.orig" ]; then
    echo "*** Error: there is no env.sh.orig in dir \"$ENV_SH_BASEDIR!\"" >&2
    exit 1
fi

cat "$ENV_SH_BASEDIR/env.sh.orig" | sed 's/-Wl,-O2//' | sed 's/-Wl,-O1//' | sed 's/-O2//' | sed 's/-O1//' > "$ENV_SH_BASEDIR/env.sh"

