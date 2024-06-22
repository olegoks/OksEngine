#!/bin/bash
#set -x
scriptDirectory=$(dirname $0)
cd ${scriptDirectory}
cd ../
mkdir Build
cd Build
cmake .. --graphviz=DependenceGraph/dependenciesGraph.dot
cd ../
cd Scripts/