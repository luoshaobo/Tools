#!/bin/bash

#############################################################################################
# {{{ for android
#export PATH=~/xbin_android_9_0_0:$PATH
export PATH=$PATH:/home/uidv4956/000_software/android-9.0.0_r22/out/host/linux-x86/bin

export JAVA_HOME=~/jdk/jdk1.8.0_202
export JRE_HOME=~/jdk/jdk1.8.0_202/jre
export CLASSPATH=.:$JAVA_HOME/lib:$JRE_HOME/lib:$CLASSPATH
export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH
# for android }}}

cd ~/000_software/android-9.0.0_r22
. build/envsetup.sh
OLD_PROMPT_COMMAND="$PROMPT_COMMAND"
lunch aosp_x86-eng
#PROMPT_COMMAND='echo -ne "\033]2;`uname -n`\007"'
PROMPT_COMMAND="$OLD_PROMPT_COMMAND"
