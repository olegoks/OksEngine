#!/bin/bash
scriptDirectory=$(dirname $0)
sh ${scriptDirectory}/LoadDependencies.sh
sh ${scriptDirectory}/GenerateECSBuildFiles.sh
sh ${scriptDirectory}/BuildEngine.sh
read -p "Press enter to exit."