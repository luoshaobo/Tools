#!/bin/bash

function gdb_init_content()
{
    echo "target remote :12345"
    echo "b main"
    echo "set solib-absolute-prefix ~/opt1"
    #echo "set solib-search-path"
    echo "c"
    echo "c"
}

gdb_init_content > .gdb.init

ddd -x .gdb.init --debugger arm-none-linux-gnueabi-gdb src/HMISettingApp/hmi-setting-app $@

