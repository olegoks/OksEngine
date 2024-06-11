#!/bin/bash
set -x
cd ../
git clone https://github.com/microsoft/vcpkg.git
cd ./vcpkg
./bootstrap-vcpkg.bat
echo $(pwd)
./vcpkg.exe install glfw3
./vcpkg.exe install vulkan
./vcpkg.exe install imgui
./vcpkg.exe install lua
./vcpkg.exe install luabridge3
./vcpkg.exe install glslang
./vcpkg.exe install boost
./vcpkg.exe install gtest
./vcpkg.exe install tinyobjloader
./vcpkg.exe install loguru
./vcpkg.exe install magic-enum
./vcpkg.exe install graphviz
./vcpkg.exe install vulkan-utility-libraries
cd ../Scripts/

