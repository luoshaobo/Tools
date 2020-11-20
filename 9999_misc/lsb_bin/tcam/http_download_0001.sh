#!/bin/bash

ping img.pconline.com.cn 56 4

CMD="curl -v -o ./13972202_1444874410670.jpg http://img.pconline.com.cn/images/upload/upc/tx/itbbs/1510/15/c11/13972202_1444874410670.jpg"
echo $CMD
eval $CMD
