@echo off
mkdir build
cd build
conan install ..
cmake .. -G "Visual Studio 14 Win64"
cmake --build . --config Release #ask cmake to build in Release mode
cd ..