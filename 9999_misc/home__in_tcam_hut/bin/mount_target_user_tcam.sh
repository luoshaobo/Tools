#!/bin/bash

ARG1="$1"

export DROPBEAR_PASSWORD=1 # for $REMOTE_SHELL

if [ "$ARG1" = "-d" ]; then
    rm -f $HOME/.ssh/known_hosts
    sudo umount /home/uidv4956/opt1
fi

sshfs $@ -o nonempty -o allow_other -o ssh_command="dbclient -p 50022" -o sftp_server=/usr/libexec/sftp-server root@127.0.0.1:/ /home/uidv4956/opt1
#sshfs -d -o nonempty -o ssh_command="dbclient -p 50022" -o sftp_server=/usr/libexec/sftp-server root@127.0.0.1:/ /home/uidv4956/opt1
