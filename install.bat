@echo off

REM %1: CMake build type [Debug, Release, RelWithDebInfo]

if "%1"=="" (
    echo %1 must be one of {Debug, Release, RelWithDebInfo}
) else (
    conan install . --build missing -s build_type=%1 --profile conanprofile.txt
    cmake --preset conan-default
)