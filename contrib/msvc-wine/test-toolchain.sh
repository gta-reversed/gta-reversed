#!/bin/bash
# Originally made by j0y/gta-reversed-diff-test
set -euo pipefail

# Quick sanity check: verify msvc-wine toolchain can compile C++23
# Run this first before attempting the full build.

echo "=== Toolchain verification ==="

# Test 1: cl.exe is callable
echo "1. Testing cl.exe..."
/opt/msvc/bin/x86/cl /nologo /? > /dev/null 2>&1 && echo "   cl.exe: OK" || {
    echo "   cl.exe: FAILED - check msvc-wine installation"
    exit 1
}

# Test 2: Basic C++23 compilation
echo "2. Testing C++23 compilation..."
cat > /tmp/test1.cpp << 'EOF'
#include <ranges>
#include <span>
#include <string_view>
#include <optional>
#include <variant>

constexpr auto square = [](int x) { return x * x; };

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    auto even_squares = std::span(arr)
        | std::views::filter([](int x) { return x % 2 == 0; })
        | std::views::transform(square);
    return 0;
}
EOF
/opt/msvc/bin/x86/cl /std:c++latest /EHsc /nologo /c /tmp/test1.cpp /Fo/tmp/test1.obj
echo "   C++23 compile: OK"

# Test 3: Linking works
echo "3. Testing linking..."
/opt/msvc/bin/x86/link /nologo /OUT:/tmp/test1.exe /tmp/test1.obj
echo "   link.exe: OK"

# Test 4: DLL creation (since .asi is a DLL)
echo "4. Testing DLL creation..."
cat > /tmp/test_dll.cpp << 'EOF'
#include <windows.h>

extern "C" __declspec(dllexport)
int __stdcall TestFunc(int x) {
    return x + 1;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return TRUE;
}
EOF
/opt/msvc/bin/x86/cl /std:c++latest /EHsc /nologo /LD /tmp/test_dll.cpp /Fe/tmp/test_dll.dll \
    /link /SUBSYSTEM:WINDOWS /DLL
echo "   DLL build: OK"

# Test 5: Windows SDK headers available
echo "5. Testing Windows SDK headers..."
cat > /tmp/test_sdk.cpp << 'EOF'
#include <windows.h>
#include <d3d9.h>
#include <dinput.h>
#include <dsound.h>
int main() { return 0; }
EOF
/opt/msvc/bin/x86/cl /std:c++latest /EHsc /nologo /c /tmp/test_sdk.cpp /Fo/tmp/test_sdk.obj
echo "   Windows SDK: OK"

# Test 6: CMake + Ninja available
echo "6. Testing CMake + Ninja..."
cmake --version | head -1
ninja --version
echo "   Build tools: OK"

# Test 7: Conan available
echo "7. Testing Conan..."
conan --version
echo "   Conan: OK"

echo ""
echo "=== All checks passed ==="

rm test_dll.obj
