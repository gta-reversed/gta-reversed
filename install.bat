@echo off

REM %1: CMake build type [Debug (default), Release, RelWithDebInfo]
REM %2: CMake profile [conan-default-unity (default), conan-default]

set BUILDTYPE=%1
if "%BUILDTYPE%"=="" set BUILDTYPE=Debug
set PROFILETYPE=%2
if "%PROFILETYPE%"=="" set PROFILETYPE=conan-default-unity
conan install . --build missing -s build_type="%BUILDTYPE%" --profile conanprofile.txt
cmake --preset "%PROFILETYPE%"
PAUSE
