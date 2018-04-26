#!/bin/bash

qmake -r ../SYS_HMI/hmi-setting/hmi-setting.pro CONFIG+=without_simulator CONFIG+=dlt_backend $@
