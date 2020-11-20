@echo off

cd "C:\Program Files\SSHFS-Win\bin"
set CYGFUSE=WinFsp
set PATH=C:\Program Files\SSHFS-Win\bin;%PATH%

echo sshfs -f -o idmap=user root@198.18.32.17:/ U:
sshfs -f -o idmap=user root@198.18.32.17:/ U:
