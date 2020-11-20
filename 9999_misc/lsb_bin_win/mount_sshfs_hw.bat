@echo off

cd "C:\Program Files\SSHFS-Win\bin"
set CYGFUSE=WinFsp
set PATH=C:\Program Files\SSHFS-Win\bin;D:\my_bin_win;%PATH%

#echo sshfs -f -o idmap=user -p 3322 -o ssh_command="ssh -i D:/lsb/keys/Identity" luoshaobo@127.0.0.1: T:
sshfs -f -o idmap=user -p 3322 -o ssh_command="ssh -i D:/lsb/keys/Identity" luoshaobo@127.0.0.1: T: