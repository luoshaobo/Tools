#!/bin/bash

GEN_VRPJ_FILENAME="HMI_DPCA_adaptor.vrpj"

SEARCH_PATHS=`cat <<EOF
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Infrastructure
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\Controls
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Infrastructure
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Toolbox_DPCA
D:\casdev\WinCE\public\MMP_PROD\_HS\HMI\Controls
EOF
`

CONTENT_PART1__HMI_DPCA_test_vrpj=`cat <<EOF
<?xml version="1.0" encoding="UTF-8"?>

<!DOCTYPE project PUBLIC "-//PresagisCanada//DTD VapsXT 14.0" "file:///C:/VAPSXTDTD/VapsXT_14.dtd">

<project name="$GEN_VRPJ_FILENAME">
  <file name="Toolbox_DPCA.vprj" path="..\HMI\Toolbox_DPCA"/>
  <file name="Toolbox_VAPS.vprj" path="..\HMI\Toolbox_VAPS" icon_path="%VAPSXT_DIR%\\Projects\\StandardElements\\Icons\\StandardElements_prj.bmp"/>
  <file name="Tables.vprj" path="..\HMI\Tables" icon_path="%VAPSXT_DIR%\\Projects\\StandardElements\\Icons\\Tables_prj.bmp"/>
  <file name="Infrastructure.vprj" path="..\HMI\Infrastructure"/>
  
  <!-- {{{ Customized -->
EOF
`

CONTENT_PART2__HMI_DPCA_test_vrpj=`cat <<EOF
  <!-- Customized }}} -->
  
  <root_prj_facet_list>
    <facet name="CodeGenOptions">
      <prop name="ImageTableGeneration">TableOnly</prop>
      <prop name="ImagePixelFormat">R8_G8_B8_A8</prop>
      <prop name="ImageUserDefinedPixelFormat"></prop>
      <prop name="MaxNumberOfTilesPerFile">512</prop>
      <prop name="EnableClassPartitioning">No</prop>
      <prop name="NumberOfDataMembersPerStructure">50</prop>
      <prop name="EnableCStructureGenerationForXTTypes">No</prop>
      <prop name="EnableImplicitTypeConversionWarnings">No</prop>
      <prop name="EnableStrictMemoryOperationWarnings">No</prop>
      <prop name="EnableSystemFont">No</prop>
      <prop name="GenerateDependencyObjs">No</prop>
      <prop name="MaximumNestedIfBlockLimit">100</prop>
    </facet>
    <facet name="TileMemOptions">
      <prop name="MemoryUnit">MegaBytes</prop>
      <structprop name="Image">
        <field name="TotalTileMemory">256</field>
        <field name="TileSizeInPixels">256</field>
      </structprop>
      <structprop name="RasterFont">
        <field name="MaximumNumberOfTiles">2</field>
        <field name="TileSizeWidth">1024</field>
        <field name="TileSizeHeight">1024</field>
      </structprop>
    </facet>
  </root_prj_facet_list>
</project>

EOF
`

usage()
{
    echo "Usage:"
    echo "    `basename $0` <name>[.vcls]"
    echo ""
}

IS_ADAPTOR_IN_TOOLBOX_DPCA="false"
FIND_ADAPTOR_RETURN_ADAPTOR_PATH=""
FIND_ADAPTOR_RETURN_VCPROJ_PATH=""

find_adaptor()
{
    local ADAPTOR_FILENAME
    local ADAPTOR_PATH
    local ADAPTOR_DIR
    
    ADAPTOR_FILENAME="$1"
    
    IS_ADAPTOR_IN_TOOLBOX_DPCA="false"
    FIND_ADAPTOR_RETURN_ADAPTOR_PATH=""
    FIND_ADAPTOR_RETURN_VCPROJ_PATH=""
    
    for LINEBUF in $SEARCH_PATHS; do
        ADAPTOR_PATH=`find -L "$LINEBUF" -iname "$ADAPTOR_FILENAME" | grep -i "$ADAPTOR_FILENAME" | grep -v ".001" | grep -v ".old" | grep -v " - Copy" 2>/dev/null`
        if [ -f "$ADAPTOR_PATH" ]; then
            FIND_ADAPTOR_RETURN_ADAPTOR_PATH="$ADAPTOR_PATH"
            ADAPTOR_DIR=`dirname "$ADAPTOR_PATH"`
            ADAPTOR_NAME=`basename "$ADAPTOR_PATH"`
            ADAPTOR_NAME_WITHOUT_EXT=${ADAPTOR_NAME%.*}
            
            VCPROJ_PATH=`find -L "$ADAPTOR_DIR" -iname "*$ADAPTOR_NAME_WITHOUT_EXT.vcproj" | grep -i "$ADAPTOR_NAME_WITHOUT_EXT.vcproj" 2>/dev/null`
            if [ ! -f "$VCPROJ_PATH" ]; then
                return 1
            else
                FIND_ADAPTOR_RETURN_VCPROJ_PATH="$VCPROJ_PATH"
                
                echo $LINEBUF | grep "Toolbox_DPCA" >/dev/null 2>&1
                
                if [ $? -eq 0 ]; then
                    IS_ADAPTOR_IN_TOOLBOX_DPCA="true"
                fi
                return 0
            fi
        fi
    done

    return 1
}

gen_HMI_DPCA_test_vrpj()
{
    local ADAPTOR_DIR
    local ADAPTOR_NAME
    
    ADAPTOR_DIR=`dirname "$FIND_ADAPTOR_RETURN_ADAPTOR_PATH"`
    ADAPTOR_NAME=`basename "$FIND_ADAPTOR_RETURN_ADAPTOR_PATH"`
    ADAPTOR_NAME=${ADAPTOR_NAME%.*}
    
    if [ $IS_ADAPTOR_IN_TOOLBOX_DPCA == "true" ]; then
        echo "$CONTENT_PART1__HMI_DPCA_test_vrpj" | grep -v "Toolbox_DPCA"
    else
        echo "$CONTENT_PART1__HMI_DPCA_test_vrpj"
    fi
    
    echo "  <file name=\"$ADAPTOR_NAME.vcls\" path=\"$ADAPTOR_DIR\"/>"
    
    echo "$CONTENT_PART2__HMI_DPCA_test_vrpj"
}

ADAPTORS=""
OPEN_IN_COMPARING_TOOL=false

if [ $# -ge 1 ]; then
    if [ "$1" == "--help" -o "$1" == "-h" ]; then
        usage
        exit 0
    elif [ "$1" == "--compare" -o "$1" == "-c" ]; then
        OPEN_IN_COMPARING_TOOL=true
        shift
        ADAPTORS="$*"
    else
        ADAPTORS="$*"
    fi
else
    usage
    exit 0
fi

for ADAPTOR in $ADAPTORS; do
    echo "========================================================================================="
    
    LAST_CHAR=`expr substr "$ADAPTOR" ${#ADAPTOR} 1`
    if [ "$LAST_CHAR" == "~" ]; then
        FIRST_CHARS=`expr substr "$ADAPTOR" 1 $((${#ADAPTOR}-1))`
        ADAPTOR="${FIRST_CHARS}Adaptor.vcls"
    fi
    
    DOT_POS=`expr index "$ADAPTOR" '.'`
    if [ "$DOT_POS" -eq 0 ]; then               # no extention name
        ADAPTOR="$ADAPTOR.vcls"
    fi
    
    EXT_NAME=${ADAPTOR##*.}
    if [ -z "$EXT_NAME" -o "$EXT_NAME" != "vcls" ]; then
        usage
        exit 0
    fi
    
    find_adaptor "$ADAPTOR"
    FIND_ADAPTOR_RETURN_ADAPTOR_PATH=${FIND_ADAPTOR_RETURN_ADAPTOR_PATH//\//\\}
    FIND_ADAPTOR_RETURN_VCPROJ_PATH=${FIND_ADAPTOR_RETURN_VCPROJ_PATH//\//\\}
    
    if [ $? -ne 0 ]; then
        echo "The pathname of $ADAPTOR is $FIND_ADAPTOR_RETURN_ADAPTOR_PATH"
        echo "The pathname of *.vcproj for $ADAPTOR is $FIND_ADAPTOR_RETURN_VCPROJ_PATH"
        echo "*** Failed to find the relevant directories and files for $ADAPTOR!" >&2
        exit 1
    fi
    
    echo "The pathname of $ADAPTOR is $FIND_ADAPTOR_RETURN_ADAPTOR_PATH"
    echo "The pathname of *.vcproj for $ADAPTOR is $FIND_ADAPTOR_RETURN_VCPROJ_PATH"
    
    gen_HMI_DPCA_test_vrpj > "D:\casdev\WinCE\public\MMP_PROD\_HS\HMI2\\$GEN_VRPJ_FILENAME"
    
    ADAPTOR_NAME=`basename "$FIND_ADAPTOR_RETURN_ADAPTOR_PATH"`
    echo "cmd /c \"C:\\Presagis\\VAPS_XT_32\\bin\\CodeGen.exe\" \"D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI2\\$GEN_VRPJ_FILENAME\" -codegen $ADAPTOR_NAME"
    cmd /c "C:\\Presagis\\VAPS_XT_32\\bin\\CodeGen.exe" "D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI2\\$GEN_VRPJ_FILENAME" -codegen $ADAPTOR_NAME

    if [ $OPEN_IN_COMPARING_TOOL == "true" ]; then
        ADAPTOR_SRC_DIR=`dirname "$FIND_ADAPTOR_RETURN_VCPROJ_PATH"`
        ADAPTOR_SRC_DIR=`dirname "$ADAPTOR_SRC_DIR"`
        ADAPTOR_NAME_WITHOUT_EXT=${ADAPTOR_NAME%.*}
        echo "cmd /c \"C:\\Program Files (x86)\\Beyond Compare 3\\BComp.exe\" \"D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI2\\CodedObject\\$ADAPTOR_NAME_WITHOUT_EXT\" \"$ADAPTOR_SRC_DIR\""
        cmd /c "C:\\Program Files (x86)\\Beyond Compare 3\\BComp.exe" "D:\\casdev\\WinCE\\public\\MMP_PROD\\_HS\\HMI2\\CodedObject\\$ADAPTOR_NAME_WITHOUT_EXT" "$ADAPTOR_SRC_DIR"
    fi
done
