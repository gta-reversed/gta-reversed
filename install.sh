#!/bin/bash

# $1: CMake build type [Debug, Release, RelWithDebInfo]

conan install . --build missing -s build_type=${1:-Debug} --profile conanprofile.txt;
cmake --preset conan-default;
