#!/bin/bash

######################################################################################
## some standard environment variables
##
#export ADK_PREFIX="/PROJ/oip/ADK/20150727_PDK_10.08.30.42/imx6x-std-adk"
#export ADK_TOOL_PREFIX="/PROJ/oip/SDK/MG_20150109_M10_6t.1.36patch/MV_Tools"

######################################################################################
## my environment variables
##
export ADK_PREFIX="/home/uidv4956/work/dev_env/gwm_new/SyDK/NAC_CN/imx6x-std-adk"
export ADK_TOOL_PREFIX="/home/uidv4956/work/dev_env/gwm_new/MV_Tools"

. $ADK_PREFIX/bin/env.sh

export PATH=$ADK_TOOL_PREFIX/tools/arm-gnueabi/bin:$PATH

