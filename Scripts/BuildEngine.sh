#!/bin/bash
set -x
cd ../
mkdir Build
cmake --build ./Build --target OksEngine
cd Scripts/
