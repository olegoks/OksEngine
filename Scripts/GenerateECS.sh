#!/bin/bash
scriptDirectory=$(dirname $0)
${scriptDirectory}/../Build/Tools/ECSGenerator/Debug/ECSGenerator.exe -workDir ${scriptDirectory}/../Sources/Engine/Sources/ -cfg ${scriptDirectory}/../Sources/Engine/Sources/ecs_gen_config.lua
