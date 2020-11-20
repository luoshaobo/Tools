#!/bin/bash

#sshfs -p 40022 -o nonempty root@127.0.0.1:/ /home/uidv4956/opt1
sshfs -p 22 -o nonempty uidv4956@hid8175u:/home/uidv4956 /home/uidv4956/hid8175u
sshfs -p 22 -o nonempty uidv4956@hid8175u:/DATA3/uidv4956 /home/uidv4956/hid8175u_DATA3

