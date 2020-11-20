#!/bin/bash

function usage
{
    echo "Usage:"
    echo "  VER=<version> . ~/bin/apply_adk_env.sh"
}
    
function main
{
    local LINE_COUNT
    local ORIG_PATH
    
    ORIG_PATH="$PATH"
    
    echo "$ORIG_PATH"
    
    if [ -z "${VERSION}" ]; then
        echo "*** ERROR: the version is null" >&2
        usage
        return 1;
    fi
    
    ls ~/SyDK_${VERSION}_*/NAVI_NAC_CN/imx6x-std-adk/bin/env.sh >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the version is wrong: $VERSION" >&2
        usage
        return 1;
    fi
    
    LINE_COUNT=`ls ~/SyDK_${VERSION}_*/NAVI_NAC_CN/imx6x-std-adk/bin/env.sh | wc -l`
    if [ "$LINE_COUNT" -ne 1 ]; then
        echo "*** ERROR: two many ADKs with the same version: $VERSION" >&2
        usage
        return 1;
    fi

    . ~/SyDK_$VERSION_*/NAVI_NAC_CN/imx6x-std-adk/bin/env.sh

    export PATH=$ORIG_PATH:$PATH
}

main
