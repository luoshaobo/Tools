#!/bin/bash

find -name ".vs" | xargs rm -fr
find -name "x64" | xargs rm -fr
find -name "log.txt" | xargs rm -fr
