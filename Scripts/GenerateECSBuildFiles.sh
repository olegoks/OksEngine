#!/bin/bash
sh RemoveAutoFiles.sh
#sh GenerateBuildFilesFromScratch.sh
scriptDirectory=$(dirname $0)
cd ${scriptDirectory}
cd ../
mkdir Build
cd Build
#cmake --build . --target ECSGenerator
cd ../
cd Scripts/
sh GenerateECS_Release.sh
sh GenerateBuildFiles.sh
read -p "Press enter to exit."
