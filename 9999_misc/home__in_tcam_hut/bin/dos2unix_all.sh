#!/bin/bash

find . -type f -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.qml" -o -name "*.pro"  -o -name "*.pri" -o -name "*.bb" -o -name "*.ts" | xargs dos2unix
