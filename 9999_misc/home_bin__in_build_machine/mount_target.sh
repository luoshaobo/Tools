#!/bin/bash

sshfs -o allow_other -o nonempty root@192.168.1.112:/ /opt1
