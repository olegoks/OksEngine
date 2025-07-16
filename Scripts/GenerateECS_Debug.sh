#!/bin/bash
scriptDirectory=$(dirname $0)
sh ${scriptDirectory}/RemoveAutoFiles.sh
${scriptDirectory}/../Build/Tools/ECSGenerator2/Debug/ECSGenerator2.exe -workDir ${scriptDirectory}/../Sources/Engine/Sources/ -cfg ${scriptDirectory}/../Sources/Engine/Sources/ecs_gen_config.lua -includeDir ${scriptDirectory}/../Sources/Engine/Sources/

sh ${scriptDirectory}/GenerateBuildFiles.sh
read -p "Press enter to exit."
