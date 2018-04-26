#!/bin/bash

is_file_last_modified_before_today()
{
    local RET
    local FILEPATH
    local NOW_YEAR
    local NOW_DAY_IN_YEAR
    local FILE_YEAR
    local FILE_DAY_IN_YEAR
    
    RET=1
    FILEPATH="$1"
        
    NOW_YEAR=$(date "+%-Y")
    NOW_DAY_IN_YEAR=$(date "+%-j")
    
    FILE_YEAR=$(date "+%-Y" -r "$FILEPATH")
    FILE_DAY_IN_YEAR=$(date "+%-j" -r "$FILEPATH")
    
    #echo "###\$NOW_YEAR=$NOW_YEAR, \$NOW_DAY_IN_YEAR=$NOW_DAY_IN_YEAR"  ###
    #echo "###\$FILE_YEAR=$FILE_YEAR, \$FILE_DAY_IN_YEAR=$FILE_DAY_IN_YEAR"  ###
    
    if (($NOW_YEAR > $FILE_YEAR || $NOW_DAY_IN_YEAR > $FILE_DAY_IN_YEAR)); then
        echo "++++"
        RET=0
    fi
    
    return $RET
}

is_file_last_modified_before_today "D:\\lsb_bin\\~build_adaptor_win_dll.cache"
