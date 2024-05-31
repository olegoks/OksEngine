#!/bin/bash
#set -x
scriptDirectory=$(dirname $0)
sh ${scriptDirectory}/LoadDependencies.sh
cd ../
mkdir Build
cd Build
cmake .. --graphviz=DependenceGraph/dependenciesGraph.dot
cd ../
cd Scripts/