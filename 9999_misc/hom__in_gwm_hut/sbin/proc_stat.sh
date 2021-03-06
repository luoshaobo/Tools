#!/bin/sh

#echo "output line info: cmd, pid, ppid, pgid, sid, tty_dev, tty_pgid"

PROCS=`ls -d /proc/*/`

for P in $PROCS; do
    if [ ! -f $P/stat ]; then
        continue
    fi
    cat $P/stat | awk '{ print $2,$1,$4,$5,$6,$7,$8 }'
    
    TASTKS=`ls -d $P/task/*/`
    for T in $TASTKS; do
        if [ ! -f $T/stat ]; then
            continue
        fi
        cat $T/stat | awk '{ print "  ", $2,$1,$4,$5,$6,$7,$8 }'
    done
done
