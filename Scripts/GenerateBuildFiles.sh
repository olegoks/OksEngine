#!/bin/bash
#set -x

cd ../
mkdir Build
cd Build
cmake .. --graphviz=foo.dot
cd ../
cd Scripts/