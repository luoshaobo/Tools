#!/bin/sh

echo "=================================================: $0"
echo "pid=$$"
echo "arguments: $@"

export AAA="aaa"
BBB="bbb"

set

exec ./child.sh $@
