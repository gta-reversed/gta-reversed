#!/bin/bash

# $1: CMake build type [Debug, Release, RelWithDebInfo]

if [ -z $1 ]; then
    echo "$1 must be one of {Debug, Release, RelWithDebInfo}";
else
    conan install . --build missing -s build_type=$1 --profile conanprofile.txt;
    cmake --preset conan-default;
fi
