#!/bin/bash
#set -x
sh GenerateBuildFiles.sh
sh BuildAll.sh
sh Test.sh
if [ $? -ne 0 ]
then
    exit -1
fi