#!/bin/bash

nohup dropbear -r ~/usr/etc/dropbear_rsa_host_key -F -p 22222 &
