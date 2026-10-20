#!/bin/bash
scriptDirectory=$(dirname $0)
sh D:/Downloads/Desktop/ECSGenerator/RemoveAutoFiles.sh
${scriptDirectory}/../Builds/ecs_generator3/ecs_generator3.exe -ecs_project_file D:/Downloads/Desktop/ECSGenerator/ecs_generator3.ecs_project

sh D:/Downloads/Desktop/ECSGenerator/GenerateECSBuildFiles.sh
read -p "Press enter to exit."
