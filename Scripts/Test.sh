#!/bin/bash
set -x
cd ../
cd ./Build
ctest -C Debug
cd ..
cd Scripts/
