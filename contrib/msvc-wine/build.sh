#!/bin/bash
# Originally made by j0y/gta-reversed-diff-test

set -euo pipefail

# Build gta-reversed using MSVC via Wine inside Docker container

SRC_DIR="."
BUILD_DIR="build"
CONAN_PROFILE="conanprofile-wine.txt"

echo "=== Phase 0: gta-reversed build via msvc-wine ==="
echo "Source: ${SRC_DIR}"
echo "Build:  ${BUILD_DIR}"

# ---- Step 1: Verify MSVC toolchain works ----
echo ""
echo "--- Verifying MSVC toolchain ---"
cat > /tmp/test_cpp23.cpp << 'EOF'
#include <ranges>
#include <span>
int main() {
    int arr[] = {1, 2, 3};
    auto v = std::span(arr);
    return 0;
}
EOF

/opt/msvc/bin/x86/cl /std:c++latest /EHsc /nologo /c /tmp/test_cpp23.cpp /Fo/tmp/test_cpp23.obj
echo "C++23 compilation: OK"

# ---- Step 2: Install Conan dependencies ----
echo ""
echo "--- Installing Conan dependencies ---"

conan install . \
    --build=missing \
    -s build_type=Debug \
    --profile "${CONAN_PROFILE}"

# ---- Step 3: CMake configure ----
echo ""
echo "--- CMake configure ---"
cmake --preset conan-debug -DGTASA_UNITY_BUILD=ON -DGTASA_NO_EDIT_AND_CONTINUE=ON

# ---- Step 3b: Fix generated file paths for Wine ----
# CMake generates #include "/unix/path" in cmake_pch.hxx and unity_*.cxx files.
# cl.exe under Wine resolves "/" as uppercase Z:\, but wine-msvc.sh converts -I
# paths to lowercase z:\. Fix: prepend z: to all Unix #include paths.
echo ""
echo "--- Fixing paths for Wine (Z: vs z: drive letter bug) ---"
PCH_HXX=$(find "." -name "cmake_pch.hxx" -path "*/CMakeFiles/*" 2>/dev/null | head -1)
if [ -n "$PCH_HXX" ]; then
    echo "  Patching PCH header: ${PCH_HXX}"
    sed -i 's|#include "/|#include "z:/|g' "$PCH_HXX"
fi
UNITY_COUNT=0
find "." -name "unity_*_cxx.cxx" -path "*/Unity/*" | while read f; do
    sed -i 's|#include "/|#include "z:/|g' "$f"
done
UNITY_COUNT=$(find "." -name "unity_*_cxx.cxx" -path "*/Unity/*" | wc -l)
echo "  Patched ${UNITY_COUNT} unity files"

# ---- Step 4: Build ----
echo ""
echo "--- Building gta_reversed.asi ---"
time cmake --build --preset conan-debug --parallel "$(nproc)"

echo ""
echo "=== Build complete ==="
find "." -name "gta_reversed.asi" -o -name "gta_reversed.dll" 2>/dev/null
if [ -d "/build" ]; then
    cp -v "."/build/Debug/source/bin/Debug/gta_reversed.asi /build/ 2>/dev/null || \
    find "." -name "gta_reversed.asi" -exec cp -v {} /build/ \; 2>/dev/null || \
    echo "(no .asi found yet)"
    find "." -name "gta_reversed.map" -exec cp -v {} /build/ \; 2>/dev/null || \
    echo "(no .map file found)"
fi
