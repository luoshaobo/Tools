#!/bin/bash

IS_STEP3=true
IS_STEP4=true

MY_IMG_ROOT_DIR="D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI\\Tables\\Images"
ORIG_IMG_ROOT_DIR="D:\\temp\\pic\\Assets"

function is_images_dir()
{
    local IMAGES_DIR
    
    IMAGES_DIR="$1"
    
    if [ ! -d "$IMAGES_DIR\\Common" ]; then
        return 1;
    fi
    
    if [ ! -d "$IMAGES_DIR\\Blue Theme" ]; then
        return 1;
    fi
    
    if [ ! -d "$IMAGES_DIR\\Red Theme" ]; then
        return 1;
    fi
    
    if [ ! -d "$IMAGES_DIR\\White Theme" ]; then
        return 1;
    fi
    
    if [ ! -d "$IMAGES_DIR\\Background" ]; then
        return 1;
    fi
    
    if [ ! -d "$IMAGES_DIR\\DF Theme" ]; then
        return 1;
    fi
    
    return 0;
}

function check_file()
{
    local FILE
    local FILE_T
    local SUBDIR
    
    FILE="$1"
    SUBDIR="$2"
    
    if [ -f "$ORIG_IMG_ROOT_DIR\\$SUBDIR\\$FILE" ]; then
        if [ ! -f "$MY_IMG_ROOT_DIR\\$SUBDIR\\$FILE" ]; then
            if [ ! -f "$MY_IMG_ROOT_DIR\\Common\\$FILE" ]; then
                echo "*** NOTE: \"$MY_IMG_ROOT_DIR\\$SUBDIR\\$FILE_T\" does not exist and \"$MY_IMG_ROOT_DIR\\Common\\$FILE_T\" does not exist!" | sed -e 's/\.\///g' | sed -e 's/\//\\/g'
            else
                echo "### NOTE: \"$MY_IMG_ROOT_DIR\\$SUBDIR\\$FILE\" does not exist!" | sed -e 's/\.\///g' | sed -e 's/\//\\/g'
            fi
        fi
    fi
}

is_images_dir "$MY_IMG_ROOT_DIR"
if [ $? -ne 0 ]; then
    echo "*** ERROR: \"$MY_IMG_ROOT_DIR\" is not a valid images dir!" >&2
    exit 1
fi

is_images_dir "$ORIG_IMG_ROOT_DIR"
if [ $? -ne 0 ]; then
    echo "*** ERROR: \"$ORIG_IMG_ROOT_DIR\" is not a valid images dir!" >&2
    exit 1
fi

FILES_FOR_CHECK=$(cd "$MY_IMG_ROOT_DIR\\Common" && find)
for FILE in $FILES_FOR_CHECK; do
    check_file "$FILE" "Common"
    check_file "$FILE" "Blue Theme"
    check_file "$FILE" "Red Theme"
    check_file "$FILE" "White Theme"
    # if [ "$IS_STEP3" == "true" ]; then
        # check_file "$FILE" "WhiteRed Theme"
    # fi
    if [ "$IS_STEP3" == "true" ]; then
        check_file "$FILE" "DF Theme"
    fi
done
