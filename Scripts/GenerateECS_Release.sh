#!/bin/bash
scriptDirectory=$(dirname $0)
#sh ${scriptDirectory}/RemoveAutoFiles.sh
${scriptDirectory}/../Build/Tools/ECSGenerator/Release/ECSGenerator.exe -workDir ${scriptDirectory}/../Sources/Engine/Sources/ -cfg ${scriptDirectory}/../Sources/Engine/Sources/ecs_gen_config.lua

