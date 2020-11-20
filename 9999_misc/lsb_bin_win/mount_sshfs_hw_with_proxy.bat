@echo off

cd "C:\Program Files\SSHFS-Win\bin"
set CYGFUSE=WinFsp
set PATH=C:\Program Files\SSHFS-Win\bin;D:\my_bin_win;%PATH%

echo start /b connect -P 54322 -H 199.19.250.205:80 119.3.91.17 22
start /b connect -P 54322 -H 199.19.250.205:80 119.3.91.17 22
echo sshfs -f -o idmap=user -p 54322 -o ssh_command="ssh -i D:/lsb/keys/Identity" luoshaobo@127.0.0.1: T:
sshfs -f -o idmap=user -p 54322 -o ssh_command="ssh -i D:/lsb/keys/Identity" luoshaobo@127.0.0.1: T:
