#!/bin/bash
scriptDirectory=$(dirname $0)
sh ${scriptDirectory}/RemoveAutoFiles_ECSGenerator3.sh
${scriptDirectory}/../Builds/ECSGenerator3/ECSGenerator3.exe -ecs_project_file D:/Downloads/Desktop/ECSGenerator/ecs_generator3.ecs_project

sh ${scriptDirectory}/GenerateBuildFiles.sh
read -p "Press enter to exit."
