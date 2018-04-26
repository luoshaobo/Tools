#!/bin/bash

######################################################################################
## some standard environment variables
##
#export ADK_PREFIX="/PROJ/oip/ADK/20150727_PDK_10.08.30.42/imx6x-std-adk"
#export ADK_TOOL_PREFIX="/PROJ/oip/SDK/MG_20150109_M10_6t.1.36patch/MV_Tools"

######################################################################################
## my environment variables
##
export ADK_PREFIX="/home/uidv4956/ADK"
export ADK_TOOL_PREFIX=="/home/uidv4956/MV_Tools"

. $ADK_PREFIX/bin/env.sh
export QML_IMPORT_PATH=/home/uidv4956/Qt5.4.0/5.4/gcc_64/imports

export PATH="/home/uidv4956/Qt5.4.0/Tools/QtCreator/bin":$PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/lib/x86_64-linux-gnu:/lib/x86_32-linux-gnu:/home/uidv4956/ADK/x86_64-linux/usr/lib
export PATH=/home/uidv4956/MV_Tools/tools/arm-gnueabi/bin:$PATH
export PATH=/home/uidv4956/Qt5.4.0/5.4/gcc_64/bin:$PATH
