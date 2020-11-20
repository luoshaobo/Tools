@echo off

cd "C:\Program Files\SSHFS-Win\bin"
set CYGFUSE=WinFsp
set PATH=C:\Program Files\SSHFS-Win\bin;%PATH%

echo sshfs -f -o idmap=user uidv4956@hid8133u: W:
sshfs -f -o idmap=user -o ssh_command="ssh -i D:/lsb/keys/Identity" uidv4956@hid8133u: W:
