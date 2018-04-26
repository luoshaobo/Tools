#!/bin/bash

function usage()
{
    echo "Usage:"
    echo "    `basename $0` <CE_tools_dir__win_format> <CE_moudle_building_tool_postfix> [<WinCE_root_dir>]"
    echo ""
}

if [ $# -lt 2 ]; then
    usage
    exit 0
fi

CE_TOOLS_DIR="$1"
CE_MODULE_BUILDING_TOOL_POSTFIX="$2"
WIN_MENU_DIR__WINDOWS='D:\lsb\links'
WIN_MENU_DIR__LINUX="/cygdrive/d/lsb/links"
WINCE_ROOT_DIR="/cygdrive/d/casdev/WinCE"

if [ $# -ge 3 ]; then
    WINCE_ROOT_DIR=$(cd "$3" && pwd)
    if [ ! -f "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat" ]; then
        echo "*** ERROR: the file \"$WINCE_ROOT_DIR/private/BAT/bldwnd.bat\" doesn't exits!" >&2
        exit 1
    fi
fi

CE_MODULE_BUILDING_TOOL_FILENAME="build_DPCA_module_dev__$CE_MODULE_BUILDING_TOOL_POSTFIX.bat"

if [ ! -d $CE_TOOLS_DIR ]; then
    echo "*** ERROR: the directory \"$CE_TOOLS_DIR\" does not exist or it is not a CE tool directroy!" >&2
    exit 1
fi

if [ ! -d $WIN_MENU_DIR__LINUX ]; then
    echo "*** ERROR: the directory for saving the windows links does not exists!" >&2
    exit 2
fi

if [ ! -f "$CE_TOOLS_DIR/build_DPCA_module_dev.bat.orig" ]; then
    mv -f "$CE_TOOLS_DIR/build_DPCA_module_dev.bat" "$CE_TOOLS_DIR/build_DPCA_module_dev.bat.orig"
fi

if [ ! -f "$CE_TOOLS_DIR/build_Image.bat.orig" ]; then
    mv -f "$CE_TOOLS_DIR/build_Image.bat" "$CE_TOOLS_DIR/build_Image.bat.orig"
fi

if [ ! -f "$CE_TOOLS_DIR/build_Image.bat.orig" ]; then
    mv -f "$CE_TOOLS_DIR/build_Image.bat" "$CE_TOOLS_DIR/build_Image.bat.orig"
fi

if [ ! -f "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat.orig" ]; then
    mv -f "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat" "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat.orig"
fi

echo "111"

> "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat"
cat "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat.orig" | sed -e 's/if "%_NOBUILDTITLE%"=="" title Windows CE Build environment for %3 %4 %WINCEDEBUG%/if "%_NOBUILDTITLE%"=="" title Building WinCE: %BUILD_TAG%/g' | sed -e 's/if "%_NOBUILDTITLE%"=="" title Windows CE Build environment for %3-%_TGTCONF% %4 %WINCEDEBUG%/if "%_NOBUILDTITLE%"=="" title Building WinCE: %BUILD_TAG%/g' >> "$WINCE_ROOT_DIR/private/BAT/bldwnd.bat"

echo "222"

> "$CE_TOOLS_DIR/$CE_MODULE_BUILDING_TOOL_FILENAME"
echo "set ENABLE_OPTIMIZER=0" >> "$CE_TOOLS_DIR/$CE_MODULE_BUILDING_TOOL_FILENAME"
echo "set BUILD_TAG=$CE_MODULE_BUILDING_TOOL_POSTFIX" >> "$CE_TOOLS_DIR/$CE_MODULE_BUILDING_TOOL_FILENAME"
echo "" >> "$CE_TOOLS_DIR/$CE_MODULE_BUILDING_TOOL_FILENAME"
sed -e 's/set \/P USER_INPUT= \^>/rem set \/P USER_INPUT= \^>\r\nset USER_INPUT=1\r\n/g' "$CE_TOOLS_DIR/build_DPCA_module_dev.bat.orig" >> "$CE_TOOLS_DIR/$CE_MODULE_BUILDING_TOOL_FILENAME"

echo "333"

> "$CE_TOOLS_DIR/build_Image.bat"
sed -e 's/set \/P USER_INPUT= \^>/rem set \/P USER_INPUT= \^>\r\nset USER_INPUT=2\r\n/g' "$CE_TOOLS_DIR/build_Image.bat.orig" >> "$CE_TOOLS_DIR/build_Image.bat"

echo "444"

cmd /c 'D:\cygwin_root\home\uidv4956\bin\create_win_shortcut.vbs' "$WIN_MENU_DIR__WINDOWS" "$CE_MODULE_BUILDING_TOOL_FILENAME" "$CE_TOOLS_DIR" "$CE_MODULE_BUILDING_TOOL_FILENAME"

echo "555"

