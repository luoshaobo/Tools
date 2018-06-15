#!/bin/bash

function main
{
    MOZILA_FIREFOX_TITLE="Mozilla Firefox"

    #
    # to show screen with title: $MOZILA_FIREFOX_TITLE
    #
    guiistk scnshow "$MOZILA_FIREFOX_TITLE" 0 0 SSM_MIN
    guiistk scnshow "$MOZILA_FIREFOX_TITLE" 0 0 SSM_MAX
    #guiistk scnResize "$MOZILA_FIREFOX_TITLE" 0 0 0,0,800,600
    
    guiistk kbdKeyDown VK_LWIN
    guiistk kbdKeyDown VK_LEFT
    guiistk kbdKeyUp VK_LEFT
    guiistk kbdKeyUp VK_LWIN

    guiistk delay 100
    
    COUNT=`guiistk scnGetCount "$MOZILA_FIREFOX_TITLE" 0`
    if [ $? -ne 0 ]; then
        echo "*** Error: no screen with tiltle: \"$MOZILA_FIREFOX_TITLE\"" >&2
        return 1
    fi
    if [ $COUNT -ne 1 ]; then
        echo "*** Error: too may screens with tiltle: \"$MOZILA_FIREFOX_TITLE\"" >&2
        return 1
    fi

    #
    # to add a new page
    #
    RECT=`guiistk findImageRect "pattern__add_new_page.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"add new page\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=`expr $X \+ 0`
    Y=`expr $Y \+ 0`
    
    echo "$X $Y"
    
    guiistk mouseClick $X,$Y
    guiistk delay 100
        
    #
    # to input the address then go
    #
    RECT=`guiistk findImageRect "pattern__show_site_info_disabled.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"show site information\" with disabled state is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=`expr $X \+ 50`
    Y=`expr $Y \+ 10`
    
    echo "$X $Y"
    
    guiistk mouseClick $X,$Y
    guiistk kbdCtrlA
    guiistk kbdCtrlX
    guiistk kbdString "http://www.baidu.com"
    guiistk kbdKeyDown VK_RETURN
    guiistk kbdKeyUp VK_RETURN
        
}

main
