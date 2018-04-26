#!/bin/bash

FOR_REVIEW=1

function usage()
{
    echo "usage: "
    echo "    `basename $0` [--for-review]"
    echo ""
}

if [ $# -ge 1 ]; then
    if [ "$1" == "-H" -o "$1" == "--help" ]; then
        usage
        exit 0
    fi
fi

if [ $# -ge 1 ]; then
    if [ "$1" == "--for-review" ]; then
        FOR_REVIEW=0
    fi
fi

find . -name "obj" | xargs rm -fr
find . -name "obj.backup*" | xargs rm -fr
find . -name "*.log" | xargs rm -f
find . -name "old" | xargs rm -fr
find . -name "*.old" | xargs rm -fr
find . -name "*.response" | xargs rm -fr
find . -name "*.err" | xargs rm -fr
find . -name "*.wrn" | xargs rm -fr
find . -name "*.ncb" | xargs rm -fr

find . -name "GUI_DPCA.cfg" | xargs rm -fr
find . -name "GUI_DPCA.mak" | xargs rm -fr

if [ "$FOR_REVIEW" -eq 0 ]; then
    find . -name "00-additions" | xargs rm -fr
    find . -name "01_create_makefile.py" | xargs rm -fr
    find . -name "TestFormat.*" | xargs rm -fr
    
    rm -f "src/HMI2/CodeGen/GUI_DPCA/makefile.mak"
fi

ITEMS=`find . -name "New folder" | sed 's/ /^/g'`
for ITEM in $ITEMS; do
    ITEM1=`echo "$ITEM"  | sed 's/\^/ /g'`
    echo "$ITEM1"
    rm -fr "$ITEM1"
done

ITEMS=`find . -name "Thumbs.db" | sed 's/ /^/g'`
for ITEM in $ITEMS; do
    ITEM1=`echo "$ITEM"  | sed 's/\^/ /g'`
    echo "$ITEM1"
    rm -f "$ITEM1"
done

rm -fr src/HMI2/CodedObject


OBJS=$(find . -name "*.obj")
for OBJ in $OBJS; do
    echo $OBJ
    OBJ_DIR=$(dirname $OBJ)
    DEL_FILES=$(find $OBJ_DIR | grep -E -v "\.lib\$" | grep -E -v "\.dll\$")
    for DEL_FILE in $DEL_FILES; do
        if [ "$DEL_FILE" != "$OBJ_DIR" ]; then
            rm -f $DEL_FILE
        fi
    done
done

rm -fr "src/HMI2/CodeGen/TestFormat"
rm -fr "src/HMI2/CodedObject"
rm -fr "src/HMI/Controls/Navi/PoiAdaptor/src/make/Release/svn-EC9FD508"                 # ???
find "src/HMI2" -name "TestFormat.*" | xargs rm -fr

grep -v "TestFormat" "src/HMI2/Formats/Formats.vprj" > "src/HMI2/Formats/Formats.vprj.new"
unix2dos "src/HMI2/Formats/Formats.vprj.new"
mv -f "src/HMI2/Formats/Formats.vprj.new" "src/HMI2/Formats/Formats.vprj"

rm -f "src/HMI2/HMI_DPCA_for_changed_vimps.vrpj"
rm -f "src/HMI2/HMI_DPCA_adaptor.vrpj"




