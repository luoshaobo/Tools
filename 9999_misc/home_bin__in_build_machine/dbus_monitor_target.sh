#!/bin/bash

dbus-monitor --address tcp:host=192.168.1.112,port=4444 $@
