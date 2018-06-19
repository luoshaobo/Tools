#!/bin/bash

function main
{
    MOZILA_FIREFOX_TITLE="360����"

    #
    # to show screen with title: $MOZILA_FIREFOX_TITLE
    #
    #guiistk scnshow "$MOZILA_FIREFOX_TITLE" 0 0 SSM_MIN
    #guiistk scnshow "$MOZILA_FIREFOX_TITLE" 0 0 SSM_MAX
	guiistk scnshow "$MOZILA_FIREFOX_TITLE" 0 0 SSM_FG
    guiistk scnResize "$MOZILA_FIREFOX_TITLE" 0 0 0,0,960,1080
	
	#guiistk Delay 500
	   
    # guiistk kbdKeyDown VK_LWIN
    # guiistk kbdKeyDown VK_LEFT
    # guiistk kbdKeyUp VK_LEFT
    # guiistk kbdKeyUp VK_LWIN

    guiistk Delay 500
    
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
    RECT=`guiistk imgFindRect "pattern__add_new_page.PNG,pattern__add_new_page_2.PNG,pattern__add_new_page_3.PNG,pattern__add_new_page_4.PNG,pattern__add_new_page_5.PNG"`
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
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
        
    #
    # to input the address then go
    #
    RECT=`guiistk imgFindRect "pattern__star_in_toolbar.PNG"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"star in toolbar\" is not found!" >&2
		return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=`expr $X \+ 200`
    Y=`expr $Y \+ 10`
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk kbdCtrlA
    guiistk kbdCtrlX
    guiistk kbdStr "http://www.baidu.com"
    guiistk kbdKeyDown VK_RETURN
    guiistk kbdKeyUp VK_RETURN
        
}

main