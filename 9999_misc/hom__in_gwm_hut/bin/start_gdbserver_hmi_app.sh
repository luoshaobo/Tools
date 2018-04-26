#!/bin/sh

systemctl stop ovip-sys-hmi-application
gdbserver-7.9.1 :12345 /usr/bin/ovip/HMI/hmi-application

