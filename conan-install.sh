#!/bin/bash

# $1: CMake build type [Debug, Release, RelWithDebInfo]

conan install . --build missing -s build_type=$1 --profile conanprofile.txt
