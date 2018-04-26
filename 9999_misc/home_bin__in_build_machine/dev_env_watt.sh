#!/bin/bash

######################################################################################
## some standard environment variables
##
#export ADK_PREFIX="/PROJ/oip/ADK/20150727_PDK_10.08.30.42/imx6x-std-adk"
#export ADK_TOOL_PREFIX="/PROJ/oip/SDK/MG_20150109_M10_6t.1.36patch/MV_Tools"

######################################################################################
## my environment variables
##
export ADK_PREFIX="/home/uidv4956/work/dev_env/Watt/Watt-ADK/rayl-std-adk"
export ADK_TOOL_PREFIX="/home/uidv4956/work/dev_env/Watt/MG_20150109_M10_6.1.50_/MV_Tools"

. $ADK_PREFIX/bin/env.sh

export QML_IMPORT_PATH=/home/uidv4956/Qt5.2.1/5.2.1/gcc_64/imports

export PATH=/home/uidv4956/Qt5.2.1/Tools/QtCreator/bin:$PATH
#export PATH=/home/uidv4956/Qt5.2.1/5.2.1/gcc_64/bin:$PATH
export PATH=/home/uidv4956/work/dev_env/Watt/Watt-ADK/rayl-std-adk/x86_64-linux/usr/bin:$PATH

export QTDIR=~/Qt5.2.1
export PATH=$PATH:$QTDIR/5.2.1/gcc_64/bin

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/lib:/lib/x86_64-linux-gnu:/lib/x86_32-linux-gnu:$ADK_PREFIX/x86_64-linux/usr/lib
export PATH=$ADK_TOOL_PREFIX/bin:$ADK_TOOL_PREFIX/libexec/gcc/arm-none-linux-gnueabi/4.8.3:$PATH

