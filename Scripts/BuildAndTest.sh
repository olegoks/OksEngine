#!/bin/bash
set -x
sh GenerateBuildFiles.sh
sh BuildAll.sh
sh Test.sh