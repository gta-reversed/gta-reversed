@echo off
REM filepath: build.cmd
REM %1: CMake build type [Debug, Release, RelWithDebInfo]

IF "%1"=="" (
    echo Build type must be one of {Debug, Release, RelWithDebInfo}
) ELSE (
    conan install . --build missing -s build_type=%1 --profile conanprofile.txt
    cmake --preset conan-default
)
