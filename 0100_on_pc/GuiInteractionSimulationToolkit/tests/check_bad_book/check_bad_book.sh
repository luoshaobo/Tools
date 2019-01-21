#!/bin/bash

CONST_MAX_WAITING_TIME_FOR_IMAGE_TO_SHOW=30000
CONST_MAX_WAITING_TIME_TO_CHECK_IMAGE_EXISTING=500

UNICORNVIEWER_WND_TITLE="UnicornViewer"

function hide_all_screens
{
    echo "************************************ hide_all_screens ************************************"
    guiistk wndshow "$UNICORNVIEWER_WND_TITLE" 0 1 SSM_MIN
    guiistk Delay 1000
}

function close_all_documents_in_unicornviewer
{
    local RECT
    local X
    local Y
    
    while [ 1 -eq 1 ]; do
        RECT=`guiistk imgWaitShown "min_max_close_buttons_for_doc-3.png","min_max_close_buttons_for_doc.png","min_max_close_buttons_for_doc-2.png" $CONST_MAX_WAITING_TIME_TO_CHECK_IMAGE_EXISTING`
        if [ $? -ne 0 ]; then
            break
        fi
        echo "$RECT"
        X=`echo $RECT | awk '{print $1}'`
        Y=`echo $RECT | awk '{print $2}'`
        
        X=$((X+103))
        Y=$((Y+10))
        
        echo "$X $Y"
        
        guiistk mseClick $X,$Y
        guiistk Delay 200
    done
}

function check_one_book
{
    local INPUT_BOOK_PATH
    local OUTPUT_BOOK_PATH
    local COUNT
    local RECT
    local X
    local Y
    local X1
    local Y1
    
    INPUT_BOOK_PATH="$1"
    
    echo "$$INPUT_BOOK_PATH=$INPUT_BOOK_PATH"

    #
    # to show screen with title: $UNICORNVIEWER_WND_TITLE
    #
    echo "@@@ to show the window $UNICORNVIEWER_WND_TITLE"
    #guiistk wndshow "$UNICORNVIEWER_WND_TITLE" 0 1 SSM_MIN
    #guiistk wndshow "$UNICORNVIEWER_WND_TITLE" 0 1 SSM_MAX
    guiistk wndshow "$UNICORNVIEWER_WND_TITLE" 0 1 SSM_FG
    guiistk Delay 100
    #guiistk wndSetSize "$UNICORNVIEWER_WND_TITLE" 0 1 0,0,960,1080
    guiistk wndshow "$UNICORNVIEWER_WND_TITLE" 0 1 SSM_NORMAL
	
    #guiistk Delay 500
	   
    # guiistk kbdKeyDown VK_LWIN
    # guiistk kbdKeyDown VK_LEFT
    # guiistk kbdKeyUp VK_LEFT
    # guiistk kbdKeyUp VK_LWIN

    guiistk Delay 100
    
    COUNT=`guiistk wndCount "$UNICORNVIEWER_WND_TITLE" 0`
    if [ $? -ne 0 ]; then
        echo "*** ERROR: no screen with tiltle: \"$UNICORNVIEWER_WND_TITLE\"" >&2
        return 1
    fi
    if [ $COUNT -ne 1 ]; then
        echo "*** ERROR: too may screens with tiltle: \"$UNICORNVIEWER_WND_TITLE\"" >&2
        return 1
    fi
    
    
    #
    # to close all of the documents in the window $UNICORNVIEWER_WND_TITLE
    #
    echo "@@@ to close all of the documents in the window \"$UNICORNVIEWER_WND_TITLE\""
    close_all_documents_in_unicornviewer

    #
    # to find the open file toolbar button, and click on it
    #
    echo "@@@ to find the open file toolbar button, and click on it"
    RECT=`guiistk imgWaitShown "open_file_icon_on_toolbar.png" $CONST_MAX_WAITING_TIME_FOR_IMAGE_TO_SHOW`
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the button \"open_file_icon_on_toolbar.png\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+3))
    Y=$((Y+3))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
    
    #
    # to find the input box in the open file dialog, set cursor in it, then input the source file path
    #
    echo "@@@ to find the input box in the open file dialog, set cursor in it, then input the source file path"
    RECT=`guiistk imgWaitShown "input_box_in_open_file_dialog-en.png","input_box_in_open_file_dialog-cn.png" $CONST_MAX_WAITING_TIME_FOR_IMAGE_TO_SHOW`
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the button \"input_box_in_open_file_dialog.png\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+3))
    Y=$((Y+3))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
    
    guiistk cbdPutStr "$INPUT_BOOK_PATH"
    guiistk Delay 100
    guiistk kbdCombKey VK_CONTROL V
    guiistk Delay 100
    
    #
    # to find the open button in the open file dialog, and click on it
    #
    echo "@@@ to find the open button in the open file dialog, and click on it"
    RECT=`guiistk imgWaitShown "open_button_in_open_file_dialog-en.png","open_button_in_open_file_dialog-cn.png" $CONST_MAX_WAITING_TIME_FOR_IMAGE_TO_SHOW`
    if [ $? -ne 0 ]; then
        echo "*** ERROR: the button \"open_button_in_open_file_dialog.png\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+3))
    Y=$((Y+3))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
    
    #
    # to find whethter the file is failed to open, and close the failure dialog if possible
    #
    echo "@@@ to find whethter the file is failed to open, and close the failure dialog if possible"
    while [ 1 -eq 1 ]; do
        RECT=`guiistk imgWaitShown "infor_in_open_failure_dailog-en.png","infor_in_open_failure_dailog-cn.png" $CONST_MAX_WAITING_TIME_TO_CHECK_IMAGE_EXISTING`
        if [ $? -ne 0 ]; then
            break
        fi
        echo "$RECT"
        X=`echo $RECT | awk '{print $1}'`
        Y=`echo $RECT | awk '{print $2}'`
        
        X=$((X+3))
        Y=$((Y+3))
        
        echo "$X $Y"
        
        guiistk Delay 100
        
        echo "*** ERROR: the file can't be openned by UnicornViewer: \'$INPUT_BOOK_PATH\'" >&2
        echo "$INPUT_BOOK_PATH" >> "$CFG_BAD_FILE_LIST"
        
        RECT=`guiistk imgWaitShown "ok_button_in_open_failure_dailog-en.png","ok_button_in_open_failure_dailog-cn.png" $CONST_MAX_WAITING_TIME_FOR_IMAGE_TO_SHOW`
        if [ $? -ne 0 ]; then
            echo "*** ERROR: the button \"ok_button_in_open_failure_dailog.png\" is not found!" >&2
            return 1
        fi
        echo "$RECT"
        X=`echo $RECT | awk '{print $1}'`
        Y=`echo $RECT | awk '{print $2}'`
        
        X=$((X+3))
        Y=$((Y+3))
        
        echo "$X $Y"
        
        guiistk mseClick $X,$Y
        guiistk Delay 100
        
        return 0
    done
    
    #
    # to close all of the documents in the window $UNICORNVIEWER_WND_TITLE
    #
    echo "@@@ to close all of the documents in the window \"$UNICORNVIEWER_WND_TITLE\""
    close_all_documents_in_unicornviewer
    
    return 0
}

function main
{
    local FILES_COUNT
    local CURRENT_FILE_I
    local FIRST_CHAR
    local SRC_FULL_FILE_PATH
    local DST_FULL_FILE_PATH_TMP
    local DST_FULL_FILE_PATH__DIR
    local DST_FULL_FILE_PATH__BASE
    local DST_FULL_FILE_PATH__EXT
    local DST_FULL_FILE_PATH
    local DST_FILE_BASE_DIR
    local SESSION_ERRNO
    local CURRENT_TIME

    . ./config.inc
    
    > "$CFG_BAD_FILE_LIST"

    FILES_COUNT=`cat "$CFG_FILE_LIST_FILE" | wc -l`
    CURRENT_FILE_I=0

    cat "$CFG_FILE_LIST_FILE" | while read -r FILE_PATH; do
        CURRENT_FILE_I=$((CURRENT_FILE_I+1))

        #
        # ignore the line beginning with '#'
        #
        FIRST_CHAR="${FILE_PATH:0:1}"
        if [ "$FIRST_CHAR" = "#" ]; then
            continue
        fi

        SRC_FULL_FILE_PATH="$CFG_SRC_FILE_DIR_BASE$FILE_PATH"
        
        echo "\$SRC_FULL_FILE_PATH=$SRC_FULL_FILE_PATH"
        
        #
        # ignore the source file that does not exist
        #    
        if [ ! -f "$SRC_FULL_FILE_PATH" ]; then
            echo "*** ERROR: file not exist: \"$SRC_FULL_FILE_PATH\"" >&2
            continue
        fi
                
        CURRENT_TIME=`date +"%Y-%m-%d %H:%M:%S"`
        echo
        echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
        echo "@@@ [$CURRENT_TIME][$CURRENT_FILE_I/$FILES_COUNT] check_one_book \"$SRC_FULL_FILE_PATH\""
        echo "@@@"
                
        #
        # convert one file 
        #
        check_one_book "$SRC_FULL_FILE_PATH"   
        SESSION_ERRNO=$?
        
        #
        # handle how to handle the next file when error occurs
        #
        if [ $SESSION_ERRNO -ne 0 ]; then
            if [ $OPT_WHEN_ERROR_TO_GO_NEXT -eq 1 ]; then
                :
            else
                exit $SESSION_ERRNO
            fi
        fi
        
        #
        # try to minimize all of the windows when error occurs
        #
        if [ $SESSION_ERRNO -ne 0 ]; then
            hide_all_screens
        fi
    done
}

main
