#!/bin/bash

export QTDIR=/home/uidv4956/Qt5.5.1/5.5/gcc_64
export LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
export PATH=$QTDIR/bin:$PATH
export QMAKESPEC=$QTDIR/mkspecs/linux-g++

export QML_IMPORT_PATH=$QTDIR/imports
