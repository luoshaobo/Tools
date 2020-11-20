#!/bin/bash

qmake -r ../SYS_HMI/hmi-setting/hmi-setting.pro CONFIG+=without_simulator CONFIG+=dlt_backend CONFIG+=debug CONFIG+=declarative_debug CONFIG+=qml_debug $@
