#!/bin/bash

FS=`ls install_*`; for F in $FS;  do ./$F; done
