#!/bin/bash
scriptDirectory=$(dirname $0)
sh ${scriptDirectory}/GenerateBuildFiles.sh

read -p "Press enter to exit."