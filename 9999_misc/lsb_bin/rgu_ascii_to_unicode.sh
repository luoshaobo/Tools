#!/bin/bash

function usage()
{
    echo "usage:"
    echo "  `basename $0` <input_ascii_rgu_file_path> <output_unicode_rgu_file_path>"
    echo ""
}

SRC_FILEPATH="$1"
DST_FILEPATH="$2"
DST_TEMP_FILEPATH="${DST_FILEPATH}.tmp"
DST_TEMP2_FILEPATH="${DST_FILEPATH}.tmp2"
DST_TEMP3_FILEPATH="${DST_FILEPATH}.tmp3"
DST_TEMP4_FILEPATH="${DST_FILEPATH}.tmp4"

if [ $# -lt 2 ]; then
    usage
    exit 0
fi

> $DST_TEMP_FILEPATH
echo "REGEDIT4" >> $DST_TEMP_FILEPATH                                                       # registry file tag
echo "" >> $DST_TEMP_FILEPATH
cat $SRC_FILEPATH >> $DST_TEMP_FILEPATH
unix2dos $DST_TEMP_FILEPATH
cat $DST_TEMP_FILEPATH | sed -e "s/\r\r//g" | sed -r -e "s/[^=]*=multi_sz.*/&\r\n/g" > $DST_TEMP2_FILEPATH
#cat $DST_TEMP2_FILEPATH | grep -v -e "\"404\"=\"" | grep -v -e "Nokia N95 (12.01)" > $DST_TEMP3_FILEPATH        # ignore the lines which can't be parsed
cat $DST_TEMP2_FILEPATH  > $DST_TEMP3_FILEPATH
FixAguRegValueName $DST_TEMP3_FILEPATH $DST_TEMP4_FILEPATH
echo -n -e "\xFF\xFE" > $DST_FILEPATH                                                       # UCS-2LE file tag
cat $DST_TEMP4_FILEPATH | iconv -c -f GBK -t UCS-2LE >> $DST_FILEPATH
