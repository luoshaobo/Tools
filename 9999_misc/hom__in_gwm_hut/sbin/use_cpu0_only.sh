#!/bin/sh

if [ ! -d /sys/fs/cgroup ]; then  
    echo "cgroup not supported by the kernel"  
else  
    mkdir /sys/fs/cgroup/cpuset   
    mount -t cgroup -ocpuset cpuset /sys/fs/cgroup/cpuset   
    echo 1 > /sys/fs/cgroup/cpuset/cgroup.clone_children  
    mkdir /sys/fs/cgroup/cpuset/cpu0  
    mkdir /sys/fs/cgroup/cpuset/cpu1  
  
    # assign a cpu for cgroup cpu0  
    echo 0 > /sys/fs/cgroup/cpuset/cpu0/cpuset.cpus  
  
    # move outself to this new cgroup  
    echo $$ > /sys/fs/cgroup/cpuset/cpu0/tasks  
    for i in $(cat /sys/fs/cgroup/cpuset/tasks); do  
        echo $i > /sys/fs/cgroup/cpuset/cpu0/tasks || echo "failed to add pid $i/$(cat /proc/$i/comm)"  
    done  
  
    # assign a cpu for cgroup cpu1  
    echo 1 > /sys/fs/cgroup/cpuset/cpu1/cpuset.cpus  
  
    # make cpu1 exclusive so processes belonging  
    # to the other cgroup won't use it  
    echo 1 > /sys/fs/cgroup/cpuset/cpu1/cpuset.cpu_exclusive  
fi