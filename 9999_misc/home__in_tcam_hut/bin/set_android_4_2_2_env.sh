#!/bin/bash

#############################################################################################
# {{{ for android
export PATH=~/xbin_android_4_2_2:$PATH
export PATH=$PATH:/home/uidv4956/000_software/android-4.2.2/out/host/linux-x86/bin

export JAVA_HOME=~/jdk/jdk1.6.0_33
export JRE_HOME=~/jdk/jdk1.6.0_33/jre
export CLASSPATH=.:$JAVA_HOME/lib:$JRE_HOME/lib:$CLASSPATH
export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH
# for android }}}

cd ~/000_software/android-4.2.2
. build/envsetup.sh
OLD_PROMPT_COMMAND="$PROMPT_COMMAND"
lunch full-eng
#PROMPT_COMMAND='echo -ne "\033]2;`uname -n`\007"'
PROMPT_COMMAND="$OLD_PROMPT_COMMAND"
