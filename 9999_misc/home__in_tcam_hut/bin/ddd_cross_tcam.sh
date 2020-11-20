#!/bin/bash

function gdb_init_content()
{
    echo "target remote :12346"
    
    #echo "set solib-absolute-prefix ~/opt1"
    #echo "set solib-absolute-prefix /home/uidv4956/tcam_root/tcam_src_root_03.19.16.01/release/fs/devel"
    #echo "set solib-search-path"
    echo "set sysroot /home/uidv4956/tcam_root/tcam_src_root_03.19.16.01/release/fs/devel"
    #echo "set sysroot ~/opt1"
    
    echo "b main"
    #echo "b bsfwk_BusinessService.cpp:29"
    #echo "b statemachinemaster2.cpp:268"
    #echo "b rmc_global.cpp:141"
    #echo "b gly_vds_rmc_signal.cc:40"
    
    echo "b rvdc_service.cpp:124"
    
    #echo "c"
    #echo "c"
}

gdb_init_content > .gdb.init

. ~/bin/set_tcam_gcc_env.sh
ddd -x .gdb.init --debugger arm-cas-linux-gnueabi-gdb $@

