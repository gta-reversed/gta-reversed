@echo off
mkdir game
mkdir build
git submodule update --init --recursive
cmake -S . -A Win32 -B build
mklink $GTASA.sln build\GTASA.sln
