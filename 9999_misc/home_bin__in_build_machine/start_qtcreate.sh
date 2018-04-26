#!/bin/bash

#. ~/bin/dev_env.sh
#qtcreator.sh

. ~/bin/set_qt_pc_env.inc
(   
    cd /home/uidv4956/qtcreator-4.0.3/bin
    ./qtcreator.sh -noload Welcome #-noload QmlDesigner -noload QmlProfiler
)
