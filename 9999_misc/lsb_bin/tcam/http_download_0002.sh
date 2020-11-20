#!/bin/bash

ping www.syntevo.com 56 4

CMD="curl -v -k -o smartgit-win-20_1_5.zip https://www.syntevo.com/downloads/smartgit/smartgit-win-20_1_5.zip"
echo $CMD
eval $CMD
