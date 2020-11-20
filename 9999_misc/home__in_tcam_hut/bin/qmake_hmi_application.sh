#!/bin/bash

qmake -r ../SYS_HMI/sys-hmi.pro CONFIG+=without_simulator CONFIG+=dlt_backend CONFIG+=with_keyboard $@
