#!/bin/bash

echo "### setup http proxy"

(
    cd /home/uidv4956/ntlmaps-0.9.9.5
    ./main.py &
)

sleep 5

export http_proxy=http://127.0.0.1:8080
export https_proxy=http://127.0.0.1:8080

echo "### rm -f .repo/repo/hooks/commit-msg"
rm -f .repo/repo/hooks/commit-msg

echo "### .repo/manifests/ActivateProjectManifest.sh"
.repo/manifests/ActivateProjectManifest.sh

echo "### .repo/manifests/ResetProjectManifest.sh"
.repo/manifests/ResetProjectManifest.sh

echo "### repo sync -j4"
repo sync -j4

echo "### kill ntlmaps"
ntlmaps_procid=`ps aux | grep "/usr/bin/python ./main.py" | grep -v "grep" | awk '{print $2}'`
if [ -n "$ntlmaps_procid" ]; then
    kill -9 "$ntlmaps_procid"
fi
