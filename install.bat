@echo off

REM %1: CMake build type [Debug, Release, RelWithDebInfo]

set BUILDTYPE=%1
if "%BUILDTYPE%"=="" set BUILDTYPE=Debug
conan install . --build missing -s build_type=%BUILDTYPE% --profile conanprofile.txt
cmake --preset conan-default
PAUSE
