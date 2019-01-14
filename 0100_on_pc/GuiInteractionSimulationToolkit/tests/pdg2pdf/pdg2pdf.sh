#!/bin/bash

function wait_for_printing_to_finish
{
    local I
    local RECT
    
    DELAY_TIME=10000
    I=0
    while [ $I -lt 3 ]; do
        guiistk Delay $DELAY_TIME
        RECT=`guiistk imgFindRect "in_printing-en.png","in_printing-cn.png"`
        if [ $? -eq 0 ]; then
            DELAY_TIME=10000
            I=0
            continue
        fi
        
        DELAY_TIME=200
        I=$((I+1))
    done
}

function close_all_documents_in_unicornviewer
{
    local RECT
    local X
    local Y
    
    while [ 1 -eq 1 ]; do
        RECT=`guiistk imgWaitShown "min_max_close_buttons_for_doc.png","min_max_close_buttons_for_doc-2.png" 500`
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

function convert_pdg_to_pdf
{
    local INPUT_BOOK_PATH
    local OUTPUT_BOOK_PATH
    local UNICORNVIEWER_WND_TITLE
    local COUNT
    local RECT
    local X
    local Y
    local X1
    local Y1
    
    INPUT_BOOK_PATH="$1"
    OUTPUT_BOOK_PATH="$2"
    
    echo "$$INPUT_BOOK_PATH=$INPUT_BOOK_PATH"
    echo "$$OUTPUT_BOOK_PATH=$OUTPUT_BOOK_PATH"

    UNICORNVIEWER_WND_TITLE="UnicornViewer"

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

    guiistk Delay 500
    
    COUNT=`guiistk wndCount "$UNICORNVIEWER_WND_TITLE" 0`
    if [ $? -ne 0 ]; then
        echo "*** Error: no screen with tiltle: \"$UNICORNVIEWER_WND_TITLE\"" >&2
        return 1
    fi
    if [ $COUNT -ne 1 ]; then
        echo "*** Error: too may screens with tiltle: \"$UNICORNVIEWER_WND_TITLE\"" >&2
        return 1
    fi
    
    
    #
    # to close all of the documents in the window $UNICORNVIEWER_WND_TITLE
    #
    echo "@@@ to close all of the documents in the window $UNICORNVIEWER_WND_TITLE"
    close_all_documents_in_unicornviewer

    #
    # to find the open file toolbar button
    #
    echo "@@@ to find the open file toolbar button"
    RECT=`guiistk imgWaitShown "open_file_icon_on_toolbar.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"open_file_icon_on_toolbar.png\" is not found!" >&2
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
    # to find the input box in the open file dialog and set cursor in it, then input the input file path
    #
    echo "@@@ to find the input box in the open file dialog and set cursor in it, then input the input file path"
    RECT=`guiistk imgWaitShown "input_box_in_open_file_dialog-en.png","input_box_in_open_file_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"input_box_in_open_file_dialog.png\" is not found!" >&2
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
    # to find the open button in the open file dialog and click on it
    #
    echo "@@@ to find the open button in the open file dialog and click on it"
    RECT=`guiistk imgWaitShown "open_button_in_open_file_dialog-en.png","open_button_in_open_file_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"open_button_in_open_file_dialog.png\" is not found!" >&2
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
    # to find the image process button in the toolbar, and click it
    #
    echo "@@@ to find the image process button in the toolbar, and click it"
    RECT=`guiistk imgWaitShown "image_proc_button_on_toolbar.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"image_proc_button_on_toolbar.png\" is not found!" >&2
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
    # to find the contrast bar in the image process dialog, drag it to the end of the right side
    #
    echo "@@@ to find the contrast bar in the image process dialog, drag it to the end of the right side"
    RECT=`guiistk imgWaitShown "contrast_bar_in_image_proc_dialog-en.png","contrast_bar_in_image_proc_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"contrast_bar_in_image_proc_dialog.png\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+180))
    Y=$((Y+8))
    
    echo "$X $Y"
    
    X1=$((X+100))
    Y1=$((Y+0))
    
    echo "$X1 $Y1"
    
    guiistk.exe mseMove $X,$Y
    guiistk Delay 100
    guiistk.exe mseDrag $X,$Y $X1,$Y1
    guiistk Delay 500
    
    #
    # to find the OK button in the image process dialog, and click it
    #
    echo "@@@ to find the OK button in the image process dialog, and click it"
    RECT=`guiistk imgWaitShown "ok_button_in_image_proc_dialog-en.png","ok_button_in_image_proc_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"ok_button_in_image_proc_dialog.png\" is not found!" >&2
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
    # to find the printer button in the toolbar, and click it
    #
    echo "@@@ to find the printer button in the toolbar, and click it"
    RECT=`guiistk imgWaitShown "print_button_in_open_file_dialog.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"print_button_in_open_file_dialog.png\" is not found!" >&2
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
    # to find the pdf printer info in the printer dialog
    #
    echo "@@@ to find the pdf printer info in the printer dialog"
    RECT=`guiistk imgWaitShown "pdf_printer_in_printer_dialog-cn-1.png","pdf_printer_in_printer_dialog-cn-2.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"pdf_printer_in_printer_dialog.png\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X+3))
    Y=$((Y+3))
    
    echo "$X $Y"
    
    #guiistk mseClick $X,$Y
    guiistk Delay 100
    
    #
    # to find the print button in the printer dialog
    #
    echo "@@@ to find the print button in the printer dialog"
    RECT=`guiistk imgWaitShown "print_button_in_printer_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"print_button_in_printer_dialog.png\" is not found!" >&2
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
    # to find the saved file input box in the saving dialog and set cursor in it, select all, then input the output file path
    #
    echo "@@@ to find the saved file input box in the saving dialog and set cursor in it, select all, then input the output file path"
    echo "to find input_box_in_open_file_dialog.png ..."
    RECT=`guiistk imgWaitShown "saved_file_inputbox_in_saving_dialog-en.png","saved_file_inputbox_in_saving_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"saved_file_inputbox_in_saving_dialog.png\" is not found!" >&2
        return 1
    fi
    echo "$RECT"
    X=`echo $RECT | awk '{print $1}'`
    Y=`echo $RECT | awk '{print $2}'`
    
    X=$((X-10))
    Y=$((Y+3))
    
    echo "$X $Y"
    
    guiistk mseClick $X,$Y
    guiistk Delay 100
    
    guiistk kbdCombKey VK_CONTROL A
    guiistk Delay 100
    guiistk cbdPutStr "$OUTPUT_BOOK_PATH"
    guiistk Delay 100
    guiistk kbdCombKey VK_CONTROL V
    guiistk Delay 100
    
    
    #
    # to find the saved button box in the saving dialog, then click it
    #
    echo "@@@ to find the saved button box in the saving dialog, then click it"
    RECT=`guiistk imgWaitShown "saved_button_in_saving_dialog-en.png","saved_button_in_saving_dialog-cn.png"`
    if [ $? -ne 0 ]; then
        echo "*** Error: the button \"saved_button_in_saving_dialog.png\" is not found!" >&2
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
    # to find the save as dialog, click OK button on it
    #
    echo "@@@ to find the save as dialog, click OK button on it"
    while [ 1 -eq 1 ]; do
        RECT=`guiistk imgWaitShown "confirm_title_in_save_as_dialog-en.png","confirm_title_in_save_as_dialog-cn.png" 1000`
        if [ $? -ne 0 ]; then
            break
        fi
        echo "$RECT"
        X=`echo $RECT | awk '{print $1}'`
        Y=`echo $RECT | awk '{print $2}'`
        
        X=$((X+3))
        Y=$((Y+3))
        
        echo "$X $Y"
        
        #guiistk mseClick $X,$Y
        guiistk Delay 100
        
        
        RECT=`guiistk imgWaitShown "ok_button_in_save_as_dialog-en.png","ok_button_in_save_as_dialog-cn.png"`
        if [ $? -ne 0 ]; then
            echo "*** Error: the button \"ok_button_in_save_as_dialog.png\" is not found!" >&2
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
        
        break
    done
        
    #
    # wait for printing to finish
    #
    echo "@@@ wait for printing to finish"
    guiistk Delay 1000
    wait_for_printing_to_finish
    
    #
    # to close all of the documents in the window $UNICORNVIEWER_WND_TITLE
    #
    echo "@@@ to close all of the documents in the window $UNICORNVIEWER_WND_TITLE"
    close_all_documents_in_unicornviewer
    
    return 0
}

. ./config.inc

cat "$FILE_LIST_FILE" | while read -r FILE_PATH; do
    FIRST_CHAR="${FILE_LIST_FILE:0:1}"
    if [ "$FIRST_CHAR" = "#" ]; then
        continue
    fi

    FILE_PATH_LIST=`echo "$FILE_PATH_LIST" | tr "[^]" "[ ]"`
    SRC_FULL_FILE_PATH="$SRC_FILE_DIR_BASE$FILE_PATH"
    DST_FULL_FILE_PATH_TMP="$DST_FILE_DIR_BASE$FILE_PATH"
    
    DST_FULL_FILE_PATH__DIR=`dirname "$DST_FULL_FILE_PATH_TMP"`
    DST_FULL_FILE_PATH__BASE=`basename "$DST_FULL_FILE_PATH_TMP"`
    DST_FULL_FILE_PATH__BASE=`echo "$DST_FULL_FILE_PATH__BASE" | sed 's/\.[^.]*$/.pdf/g'`
    DST_FULL_FILE_PATH="$DST_FULL_FILE_PATH__DIR\\$DST_FULL_FILE_PATH__BASE"
    
    echo "\$SRC_FULL_FILE_PATH=$SRC_FULL_FILE_PATH"
    echo "\$DST_FULL_FILE_PATH=$DST_FULL_FILE_PATH"
        
    if [ ! -f "$SRC_FULL_FILE_PATH" ]; then
        echo "*** ERROR: file not exist: \"$SRC_FULL_FILE_PATH\""
        continue
    fi
    
    DST_FILE_BASE_DIR=`dirname "$DST_FULL_FILE_PATH"`
    echo "mkdir -p \"$DST_FILE_BASE_DIR\""
    mkdir -p "$DST_FILE_BASE_DIR"
    
    if [ $OPT_NOT_TO_REWRITE_EXISTING_FILE -eq 1 ]; then
        if [ -f "$DST_FULL_FILE_PATH" ]; then
            echo "### INFO: file already exist: \"$DST_FULL_FILE_PATH\""
            continue
        fi
    fi
        
    echo "@@@ convert_pdg_to_pdf \"$SRC_FULL_FILE_PATH\" \"$DST_FULL_FILE_PATH\""
    convert_pdg_to_pdf "$SRC_FULL_FILE_PATH" "$DST_FULL_FILE_PATH"
    
    if [ $? -ne 0 ]; then
        exit $?
    fi
done
