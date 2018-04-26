#!/bin/bash

TEST_FORMATT_SRC_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI2\\CodeGen\\TestFormat"
TEST_FORMATT_DST_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI2\\CodeGen\\TestFormat.OK"

mkdir -p "$TEST_FORMATT_DST_DIR"
rm -fr "$TEST_FORMATT_DST_DIR\\*"

FILE_LIST=$(cd "$TEST_FORMATT_SRC_DIR" && ls)

for FILE in $FILE_LIST; do
    echo "$FILE" | grep "TestFormat" >/dev/null 2>&1
    if [ $? -eq 0 ]; then
        continue
    fi
    
    sed 's/TestFormat_/GUI_DPCA_/g' "$TEST_FORMATT_SRC_DIR\\$FILE" > "$TEST_FORMATT_DST_DIR\\$FILE"
done

