@echo off

if not exist build mkdir build
mkdir build
cd build
cmake ..
cmake --build .
