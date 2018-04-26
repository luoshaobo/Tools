#!/bin/bash


DEST_FILE="D:\\casdev\\WinCE\\public\\MMP_PROD\\OAK\\INC\\_SYS\\EngineeringMode\\version.h"


if [ ! -r "$DEST_FILE" ]; then

    echo "*** ERROR: the source code has been not mounted!" >&2
    exit 1
fi

grep -rn "MAIN_IMX_APP_VERSION" "$DEST_FILE"

