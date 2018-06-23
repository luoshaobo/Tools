#!/bin/bash

function main
{
    MOZILA_FIREFOX_TITLE="360极速"

    #
    # to show screen with title: $MOZILA_FIREFOX_TITLE
    #
    #guiistk wndshow "$MOZILA_FIREFOX_TITLE" 0 1 SSM_MIN
    #guiistk wndshow "$MOZILA_FIREFOX_TITLE" 0 1 SSM_MAX
    guiistk wndshow "$MOZILA_FIREFOX_TITLE" 0 1 SSM_FG
    guiistk Delay 100
    guiistk wndSetSize "$MOZILA_FIREFOX_TITLE" 0 1 0,0,960,1080
    guiistk wndshow "$MOZILA_FIREFOX_TITLE" 0 1 SSM_RESTORE
	
    #guiistk Delay 500
	   
    # guiistk kbdKeyDown VK_LWIN
    # guiistk kbdKeyDown VK_LEFT
    # guiistk kbdKeyUp VK_LEFT
    # guiistk kbdKeyUp VK_LWIN

    guiistk Delay 500
    
    COUNT=`guiistk wndCount "$MOZILA_FIREFOX_TITLE" 0`
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
    RECT=`guiistk imgWaitShown "pattern__add_new_page.PNG,pattern__add_new_page_2.PNG,pattern__add_new_page_3.PNG,pattern__add_new_page_4.PNG,pattern__add_new_page_5.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"add new page\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+0))
    Y=$((Y+0))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
        
    #
    # to input the address then go
    #
    RECT=`guiistk imgWaitShown "pattern__star_in_toolbar.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"star in toolbar\" is not found!" >&2
		return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+200))
    Y=$((Y+10))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk kbdCombKey VK_CONTROL A
    guiistk kbdKey VK_DELETE
    guiistk kbdStr "http://www.baidu.com"
    guiistk kbdKey VK_RETURN
	
	guiistk Delay 500
	
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
    
    X=$((X-10))
    Y=$((Y+5))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk kbdCombKey VK_CONTROL A
    guiistk kbdKey VK_DELETE
    guiistk cbdPutStr "我的小书屋"
    guiistk kbdCombKey VK_CONTROL V
    guiistk kbdKey VK_RETURN
}

main
