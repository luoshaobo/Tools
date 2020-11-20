#!/bin/bash

ls -alh $*

time scp -2 -C -p $* luoshaobo@119.3.91.17:~/tmp/
