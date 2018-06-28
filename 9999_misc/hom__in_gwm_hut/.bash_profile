export PATH=`pwd`/bin:`pwd`/sbin:$PATH

alias pt='ping 192.168.1.111'

alias mount_remote='sshfs -o allow_other -o nonempty -o ssh_command="dbclient -i /home/root/.ssh/id_rsa" uidv4956@192.168.1.111: /opt1'
alias umount_remote='umount /opt1'

alias mrh='sshfs -o allow_other -o nonempty -o ssh_command="dbclient -i /home/root/.ssh/id_rsa" uidv4956@192.168.1.111: /home/uidv4956'
alias mrh2='sshfs -o allow_other -o nonempty -o ssh_command="dbclient -i /home/root/.ssh/id_rsa" -o sftp_server=/home/uidv4956/usr/sbin/sftp-server uidv4956@192.168.1.100: /home/uidv4956'
alias mrh2d='sshfs -o debug -o allow_other -o nonempty -o ssh_command="dbclient -i /home/root/.ssh/id_rsa" -o sftp_server=/home/uidv4956/usr/sbin/sftp-server uidv4956@192.168.1.100: /home/uidv4956'
alias mrh3='sshfs -o allow_other -o nonempty -o ssh_command="dbclient -p 10022 -i /home/root/.ssh/id_rsa" -o sftp_server=/home/uidv4956/usr/sbin/sftp-server uidv4956@127.0.0.1: /home/uidv4956'
alias mrh3d='sshfs -o debug -o allow_other -o nonempty -o ssh_command="dbclient -p 10022 -i /home/root/.ssh/id_rsa" -o sftp_server=/home/uidv4956/usr/sbin/sftp-server uidv4956@127.0.0.1: /home/uidv4956'
alias urh='umount /home/uidv4956'

export LANG=zh_CN.GB2312
export LC_ALL=zh_CN.GB2312
stty pass8

