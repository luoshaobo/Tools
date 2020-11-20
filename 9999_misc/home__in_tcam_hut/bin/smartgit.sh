#!/bin/bash --login
#
# Editing this script should not be required.
#
# To specify additional VM options, add them to smartgit.vmoptions files.

APP_NAME=SmartGit
APP_LOWER=smartgit

parseVmOptions() {
  if [ -f $1 ]; then
    while read LINE || [[ -n "$LINE" ]]; do
      LINE="${LINE#"${LINE%%[![:space:]]*}"}"
      if [ ${#LINE} -gt 0 ] && [ ! ${LINE:0:1} == '#' ]; then
        if [ ${LINE:0:4} == 'jre=' ]; then
          echo "Ignoring following line in file $1"
          echo "$LINE"
        elif [ ${LINE:0:5} == 'path=' ]; then
          APP_PATH="$APP_PATH:${LINE:5}"
        else
          _VM_PROPERTIES="$_VM_PROPERTIES $LINE"
        fi
      fi
    done < $1
  fi
}

echoJreConfigurationAndExit() {
    mkdir --parents $APP_CONFIG_DIR
    touch $APP_CONFIG_DIR/$APP_LOWER.vmoptions
    exit 1
}

case "$BASH" in
    */bash) :
        ;;
    *)
        exec /bin/bash "$0" "$@"
        ;;
esac


# check system architecture
ARCHITECTURE=`uname -m`
if [ "$ARCHITECTURE" != "x86_64" ] && [ "$ARCHITECTURE" != "aarch64" ] ; then
    echo "$APP_NAME is not supported any more on 32-bit systems."
    exit 1
fi

# Resolve the location of the SmartGit installation.
# This includes resolving any symlinks.
PRG=$0
while [ -h "$PRG" ]; do
  ls=`ls -ld "$PRG"`
  link=`expr "$ls" : '^.*-> \(.*\)$' 2>/dev/null`
  if expr "$link" : '^/' 2> /dev/null >/dev/null; then
    PRG="$link"
  else
    PRG="`dirname "$PRG"`/$link"
  fi
done

APP_BIN=`dirname "$PRG"`

# Absolutize dir
oldpwd=`pwd`
cd "${APP_BIN}";
APP_BIN=`pwd`
cd "${oldpwd}";
unset oldpwd

APP_CONFIG_DIR=${XDG_CONFIG_HOME:-$HOME/.config}/$APP_LOWER

APP_HOME=`dirname "$APP_BIN"`
APP_JAVA_HOME=$SMARTGIT_JAVA_HOME
parseVmOptions $APP_BIN/$APP_LOWER.vmoptions
parseVmOptions $HOME/.$APP_LOWER/$APP_LOWER.vmoptions
parseVmOptions $APP_CONFIG_DIR/$APP_LOWER.vmoptions

# Determine Java Runtime
_JAVA_EXEC="java"
if [ "$APP_JAVA_HOME" != "" ] ; then
    _TMP="$APP_JAVA_HOME/bin/java"
    if [ -f "$_TMP" ] ; then
        if [ -x "$_TMP" ] ; then
            _JAVA_EXEC="$_TMP"
        else
            echo "Warning: $_TMP is not executable"
        fi
    else
        echo "Warning: $_TMP does not exist"
    fi
elif [ -e "$APP_HOME/jre/bin/java" ] && [ "$ARCHITECTURE" == "x86_64" ] ; then
    _JAVA_EXEC="$APP_HOME/jre/bin/java"
fi

if ! which "$_JAVA_EXEC" >/dev/null 2>&1 ; then
    echo "Error: No Java Runtime Environment (JRE) 11 or higher found"
    echoJreConfigurationAndExit
fi

if type "lsb_release" > /dev/null 2> /dev/null ; then
    if [ "$XDG_CURRENT_DESKTOP" == "Unity" ] ; then
        # work-around for https://bugs.eclipse.org/bugs/show_bug.cgi?id=419729
        # work-around for https://bugs.eclipse.org/bugs/show_bug.cgi?id=502056
        export UBUNTU_MENUPROXY=0

        # Without the following line sliders are not visible in Ubuntu 12.04
        # (see <https://bugs.eclipse.org/bugs/show_bug.cgi?id=368929>)
        export LIBOVERLAY_SCROLLBAR=0
    fi
fi

if [ "$KDE_SESSION_UID" != "" ] && [ "$GTK2_RC_FILES" == "" ] ; then
	if grep -q "oxygen-gtk" "$HOME/.gtkrc-2.0-kde4"; then
		echo "Please change the GTK+ theme to something else than oxygen-gtk."
		echo "See also http://www.syntevo.com/blog/?p=4143"
		exit -1
	fi
fi

if [ "$XDG_SESSION_TYPE" = "wayland" ]; then
    export GDK_BACKEND=x11
fi

# work-around for https://bugs.eclipse.org/bugs/show_bug.cgi?id=542675
if [ "$GTK_IM_MODULE" == "xim" ] ; then
	export GTK_IM_MODULE=ibus
fi

#export GTK_THEME=Adwaita
if [ "$GTK_THEME" != "Adwaita" ] ; then
	echo "If you experience strange GUI bugs or crashes, try setting GTK_THEME=Adwaita."
fi

_GC_OPTS="-XX:+UseG1GC -XX:MaxGCPauseMillis=100 -XX:InitiatingHeapOccupancyPercent=25 -Xmx1024m -Xss2m"
_MISC_OPTS="-Xverify:none -XX:MaxJavaStackTraceDepth=1000000 -Dsun.io.useCanonCaches=false -XX:ErrorFile=$APP_CONFIG_DIR/20.1/hs_err_pid%p.log"

APP_PATH="$PATH$SMARTGIT_PATH"

(export PATH="$APP_PATH"; "$_JAVA_EXEC" $_GC_OPTS $_MISC_OPTS $_VM_PROPERTIES -jar "$APP_HOME/lib/bootloader.jar" "$@")
