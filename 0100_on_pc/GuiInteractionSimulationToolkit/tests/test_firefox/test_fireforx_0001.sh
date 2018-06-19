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
    
    # guiistk kbdKeyDown VK_LWIN
    # guiistk kbdKeyDown VK_LEFT
    # guiistk kbdKeyUp VK_LEFT
    # guiistk kbdKeyUp VK_LWIN

    guiistk Delay 100
    
    COUNT=`guiistk scnCount "$MOZILA_FIREFOX_TITLE" 0`
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
    RECT=`guiistk imgWaitShown "pattern__add_new_page.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"add_new_page\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=`expr $X \+ 0`
    Y=`expr $Y \+ 0`
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
        
    #
    # to input the address then go
    #
    RECT=`guiistk imgWaitShown "pattern__show_site_info_disabled.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"show_site_info_disabled\" with disabled state is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=`expr $X \+ 50`
    Y=`expr $Y \+ 10`
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk kbdCtrlA
    guiistk kbdKeyClick VK_DELETE
    guiistk kbdStr "http://www.baidu.com"
    guiistk kbdKeyClick VK_RETURN
    
    guiistk Delay 1000
    
    #
    # to input the search keyword then go
    #
    RECT=`guiistk imgWaitShown "pattern__camera_icon_on_right_of_keyword_input_box.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the icon \"camera_icon_on_right_of_keyword_input_box\" with disabled state is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=`expr $X - 10`
    Y=`expr $Y \+ 5`
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk kbdCtrlA
    guiistk kbdKeyClick VK_DELETE
    guiistk kbdStr "我的小书屋"
    guiistk kbdKeyClick VK_RETURN    
}

main
