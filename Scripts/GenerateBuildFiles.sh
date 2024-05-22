#!/bin/bash
#set -x
sh LoadDependencies.sh
cd ../
mkdir Build
cd Build
cmake .. --graphviz=DependenceGraph/dependenciesGraph.dot
cd ../
cd Scripts/