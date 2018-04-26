#!/bin/bash

ADAPTOR_PATH_PREFIX='D:\casdev\WinCE\public\MMP_PROD\_HS'
LOG_FILE_PATH="d:/adaptor_build.log"

ADAPTOR_PATHES=`cat <<EOF
HMI\Infrastructure\DataTypes\EvtID
HMI\Infrastructure\DataTypes\ScreenID
HMI\Toolbox_DPCA\ImageManager
HMI\Toolbox_DPCA\UShortImage

HMI\Controls\Util\StringSplitter

HMI2\Controls\Telematics\TlmGenAlertPopupAdapter
HMI2\Controls\Telematics\TlmGenListPopupAdapter
HMI2\Controls\Telematics\TlmGenQuestionPopupAdapter
HMI2\Controls\Telematics\TlmPopupLayout

EOF`

# HMI\Controls\Navi\MapAdaptor
# HMI\Controls\Navi\NavController
# HMI\Controls\Navi\PoiAdaptor

# HMI\Controls\Navi\AddressAdaptor
# HMI\Controls\Navi\DetourAdaptor
# HMI\Controls\Navi\FavAdaptor
# HMI\Controls\Navi\HistoryAdaptor
# HMI\Controls\Navi\KeyBoardAdaptor
# HMI\Controls\Navi\NavPopUpHandler
# HMI\Controls\Navi\RouteAdaptor
# HMI\Controls\Navi\SettingsAdaptor
# HMI\Controls\Navi\TmcAdaptor
# HMI\Controls\Navi\calGetCarretXPos


> $LOG_FILE_PATH

function build_one_adpator()
{
    ADAPTOR_PATH="$1"
    MAKE_DIR=$(find "$ADAPTOR_PATH" -name "make")
    
    dll_filename=$(cd $MAKE_DIR/Release; ls *.dll 2>/dev/null)
    if [ -z "$dll_filename" ]; then
        (
            cd $MAKE_DIR
            VCPROJ=$(ls *.vcproj)
            echo "=================================================================================="
            echo "=== in $MAKE_DIR"
            echo "=== vcbuild $VCPROJ release"
            vcbuild $VCPROJ release
        )
        echo "built: $MAKE_DIR" >> $LOG_FILE_PATH
    else
        dll_filename=$(basename $dll_filename)
        result1=$(cd $MAKE_DIR; find "$ADAPTOR_PATH" -name "*.cpp" -newer Release/$dll_filename)
        result2=$(cd $MAKE_DIR; find "$ADAPTOR_PATH" -name "*.h" -newer Release/$dll_filename)
        if [ -z "$result1" -a -z "$result2" ]; then
            echo "not_built: $MAKE_DIR" >> $LOG_FILE_PATH
        else
            (
                cd $MAKE_DIR
                VCPROJ=$(ls *.vcproj)
                echo "=================================================================================="
                echo "=== in $MAKE_DIR"
                echo "=== vcbuild $VCPROJ release"
                vcbuild $VCPROJ release
            )
            echo "built: $MAKE_DIR" >> $LOG_FILE_PATH
        fi
    
    fi
}

for ADAPTOR_PATH in $ADAPTOR_PATHES; do
    if [ ! -z "$ADAPTOR_PATH" ]; then
        echo "$ADAPTOR_PATH" | grep -E "^[ \t]*#" >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            build_one_adpator "$ADAPTOR_PATH_PREFIX\\$ADAPTOR_PATH"
        fi
    fi
done
