#!/bin/bash

# $1: CMake build type [Debug (default), Release, RelWithDebInfo]
# $2: CMake profile [conan-default-unity (default), conan-default]

conan install . --build missing -s build_type=${1:-Debug} --profile conanprofile.txt;
cmake --preset ${2:-conan-default-unity};
