### Description:
>
### Example:
```bat
mkdir build
cd build
cmake ..
::Find CMakeLists.txt in the ".." directory and generate build files in the current("build") directory

cmake -H. -Bbuild
::Find CMakeLists.txt in the current directory and generate build files in the build directory

cmake .. --target OksEngine
::Generate build files only for OksEngine target
```