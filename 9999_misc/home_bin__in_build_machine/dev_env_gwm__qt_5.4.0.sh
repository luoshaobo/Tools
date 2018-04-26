#!/bin/bash

######################################################################################
## some standard environment variables
##
#export ADK_PREFIX="/PROJ/oip/ADK/20150727_PDK_10.08.30.42/imx6x-std-adk"
#export ADK_TOOL_PREFIX="/PROJ/oip/SDK/MG_20150109_M10_6t.1.36patch/MV_Tools"

######################################################################################
## my environment variables
##
export ADK_PREFIX="/home/uidv4956/work/dev_env/gwm/SyDK/NAC_CN/imx6x-std-adk"
#export ADK_TOOL_PREFIX="/home/uidv4956/MV_Tools"

. $ADK_PREFIX/bin/env.sh
export QML_IMPORT_PATH=$QML_IMPORT_PATH:/home/uidv4956/Qt5.4.0/5.4/gcc_64/imports

### {{{
#export QML_IMPORT_PATH=$QML_IMPORT_PATH:/home/uidv4956/yyy/Nav/qml/NavigationAppHMILib
QML_SEARCH_ROOT_DIR=/opt1
QML_SEARCH_DIRS=`cat <<EOF
#/media/datastore/ui/HMIApplications/Nav/qml/NavigationAppHMILib

#/usr/lib/qt5/qml/Wave2
#/usr/lib/qt5/qml
#/media/datastore/ui/HMITemplates/Universes/Wave2/NAC_ASIA_Pixel
#/media/datastore/ui/HMITemplates/Universes/Wave2/NAC_ASIA_Pixel/Templates/components
#/media/datastore/ui/HMITemplates/Universes/Wave2/NAC_ASIA_Pixel/Templates/panels
#/media/datastore/ui/HMITemplates/Universes/Wave2/NAC_ASIA_Pixel/Templates/common_wired/components
#/media/datastore/ui/HMITemplates/Universes/Wave2/NAC_ASIA_Pixel/Templates/common_wired/panels

#/opt/test/KeyboardAsiaTestApplication/
#/opt/test/KeyboardAsiaTestApplication/qml/Layouts


EOF`
for QML_SEARCH_DIR in $QML_SEARCH_DIRS; do
    echo "$QML_SEARCH_DIR" | grep -E -e '^[[:space:]]*#' >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        export QML_IMPORT_PATH=$QML_IMPORT_PATH:$QML_SEARCH_ROOT_DIR/$QML_SEARCH_DIR
    fi
done
echo QML_IMPORT_PATH=$QML_IMPORT_PATH
### }}}
export QML2_IMPORT_PATH=$QML_IMPORT_PATH

export PATH="/home/uidv4956/Qt5.4.0/Tools/QtCreator/bin":$PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/lib/x86_64-linux-gnu:/lib/x86_32-linux-gnu:$ADK_PREFIX/x86_64-linux/usr/lib
export PATH=$ADK_TOOL_PREFIX/tools/arm-gnueabi/bin:$PATH
export PATH=/home/uidv4956/Qt5.4.0/5.4/gcc_64/bin:$PATH
export PATH=$ADK_PREFIX/x86_64-linux/usr/bin:$PATH
