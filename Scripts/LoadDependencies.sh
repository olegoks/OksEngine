#!/bin/bash
set -x
scriptDirectory=$(dirname $0)
cd ${scriptDirectory}
cd ../
git clone https://github.com/microsoft/vcpkg.git
cd ./vcpkg
./bootstrap-vcpkg.bat
echo $(pwd)
./vcpkg.exe install sfml
./vcpkg.exe install cgal
./vcpkg.exe install meshoptimizer
./vcpkg.exe install yaml-cpp
./vcpkg.exe install glfw3
./vcpkg.exe install vulkan
./vcpkg.exe install imgui[docking-experimental]
./vcpkg.exe install implot
./vcpkg.exe install lua
./vcpkg.exe install luabridge3
./vcpkg.exe install glslang
./vcpkg.exe install boost-thread
./vcpkg.exe install gtest
./vcpkg.exe install stb
./vcpkg.exe install loguru
./vcpkg.exe install magic-enum
./vcpkg.exe install graphviz
./vcpkg.exe install physx
./vcpkg.exe install glm
./vcpkg.exe install assimp
./vcpkg.exe install vulkan-utility-libraries
cd ../Scripts/

