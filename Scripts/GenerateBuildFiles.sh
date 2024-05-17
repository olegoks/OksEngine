#!/bin/bash
#set -x

cd ../
rm -d -R Build
mkdir Build
cd Build
cmake ..
cd ../
cd Scripts/