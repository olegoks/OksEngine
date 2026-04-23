#!/bin/bash
scriptDirectory=$(dirname $0)
sh ${scriptDirectory}/RemoveAutoFiles_ECSGenerator3.sh
${scriptDirectory}/../Build/Tools/ECSGenerator2/Debug/ECSGenerator2.exe -workDir ${scriptDirectory}/../Tools/ECSGenerator3/Sources/ -cfg ${scriptDirectory}/../Sources/Engine/Sources/ecs_gen_config.lua -includeDir ${scriptDirectory}/../Tools/ECSGenerator3/Sources/

sh ${scriptDirectory}/GenerateBuildFiles.sh
read -p "Press enter to exit."
