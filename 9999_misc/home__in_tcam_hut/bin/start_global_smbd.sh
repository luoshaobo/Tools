#!/bin/bash

nohup sudo socat tcp-listen:445,bind=10.214.154.151,fork tcp-connect:127.0.0.1:10445 &
