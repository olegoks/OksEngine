#!/bin/bash
#set -x
sh LoadDependencies.sh
cd ../
mkdir Build
cd Build
cmake .. --graphviz=dependenciesGraph.dot
cd ../
cd Scripts/