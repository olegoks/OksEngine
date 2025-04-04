#!/bin/bash
sh RemoveAutoFiles.sh
sh GenerateBuildFilesFromScratch.sh
scriptDirectory=$(dirname $0)
cd ${scriptDirectory}
cd ../
mkdir Build
cd Build
cmake --build . --target ECSGenerator
cd ../
cd Scripts/
sh GenerateECS.sh
sh GenerateBuildFilesFromScratch.sh
