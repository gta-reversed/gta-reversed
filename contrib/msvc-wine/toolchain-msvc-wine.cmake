# CMake toolchain file for cross-compiling with MSVC via Wine (msvc-wine)
#
# msvc-wine's install.sh creates wrapper scripts in /opt/msvc/bin/x86/
# that invoke the real cl.exe/link.exe through Wine.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86)

# msvc-wine installation root
set(MSVC_WINE_ROOT "/opt/msvc" CACHE PATH "msvc-wine installation root")

# Auto-detect MSVC and SDK versions from what's installed
file(GLOB _msvc_versions "${MSVC_WINE_ROOT}/vc/tools/msvc/*")
list(SORT _msvc_versions ORDER DESCENDING)
list(GET _msvc_versions 0 _msvc_ver_path)
get_filename_component(MSVC_VERSION "${_msvc_ver_path}" NAME)

file(GLOB _sdk_versions "${MSVC_WINE_ROOT}/kits/10/include/*")
list(SORT _sdk_versions ORDER DESCENDING)
list(GET _sdk_versions 0 _sdk_ver_path)
get_filename_component(SDK_VERSION "${_sdk_ver_path}" NAME)

message(STATUS "msvc-wine toolchain: MSVC ${MSVC_VERSION}, SDK ${SDK_VERSION}")

set(MSVC_TOOLS "${MSVC_WINE_ROOT}/vc/tools/msvc/${MSVC_VERSION}")
set(SDK_ROOT "${MSVC_WINE_ROOT}/kits/10")

# Compilers - use the wrapper scripts created by msvc-wine's install.sh
set(CMAKE_C_COMPILER   "${MSVC_WINE_ROOT}/bin/x86/cl")
set(CMAKE_CXX_COMPILER "${MSVC_WINE_ROOT}/bin/x86/cl")
set(CMAKE_LINKER       "${MSVC_WINE_ROOT}/bin/x86/link")
set(CMAKE_RC_COMPILER  "${MSVC_WINE_ROOT}/bin/x86/rc")
set(CMAKE_MT           "${MSVC_WINE_ROOT}/bin/x86/mt")
set(CMAKE_AR           "${MSVC_WINE_ROOT}/bin/x86/lib")

# Tell CMake this is MSVC with the correct version.
# The cl.exe wrapper outputs a banner through Wine stderr which CMake may
# fail to parse. Explicitly set the compiler ID and version so CMake
# knows the feature set (C++23, etc).
# cl.exe 19.50.x = MSVC toolset v150, Visual Studio 2025/2026
set(CMAKE_C_COMPILER_ID "MSVC" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER_ID "MSVC" CACHE STRING "" FORCE)
# Report as 19.40 (VS 2022 17.10) — a version CMake definitively knows.
# The feature set is identical to 19.50; this only affects CMake's
# C++ standard/feature detection tables.
set(CMAKE_C_COMPILER_VERSION "19.40" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER_VERSION "19.40" CACHE STRING "" FORCE)
set(MSVC_VERSION 1940 CACHE STRING "" FORCE)
set(MSVC_TOOLSET_VERSION 143 CACHE STRING "" FORCE)
set(MSVC TRUE CACHE BOOL "" FORCE)

# CMake's cross-compile path may skip the compiler feature probe.
# Set the computed defaults that __compiler_check_default_language_standard needs.
set(CMAKE_C_STANDARD_COMPUTED_DEFAULT "11" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "14" CACHE STRING "" FORCE)
set(CMAKE_C_EXTENSIONS_COMPUTED_DEFAULT "OFF" CACHE STRING "" FORCE)
set(CMAKE_CXX_EXTENSIONS_COMPUTED_DEFAULT "OFF" CACHE STRING "" FORCE)

# Include paths: MSVC STL + Windows SDK
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
    "${MSVC_TOOLS}/include"
    "${SDK_ROOT}/include/${SDK_VERSION}/ucrt"
    "${SDK_ROOT}/include/${SDK_VERSION}/shared"
    "${SDK_ROOT}/include/${SDK_VERSION}/um"
    "${SDK_ROOT}/include/${SDK_VERSION}/winrt"
)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_STANDARD_INCLUDE_DIRECTORIES})

# Library search paths for x86
link_directories(
    "${MSVC_TOOLS}/lib/x86"
    "${SDK_ROOT}/lib/${SDK_VERSION}/ucrt/x86"
    "${SDK_ROOT}/lib/${SDK_VERSION}/um/x86"
)

# Skip CMake's TryCompile ABI detection — it generates /Fd with an empty
# PDB path which causes C1902 errors. The compiler is known to work.
set(CMAKE_C_COMPILER_WORKS TRUE CACHE BOOL "" FORCE)
set(CMAKE_CXX_COMPILER_WORKS TRUE CACHE BOOL "" FORCE)
set(CMAKE_C_ABI_COMPILED TRUE CACHE BOOL "" FORCE)
set(CMAKE_CXX_ABI_COMPILED TRUE CACHE BOOL "" FORCE)

# Set ABI info that CMake would normally detect via TryCompile
set(CMAKE_SIZEOF_VOID_P 4 CACHE STRING "" FORCE)  # 32-bit
set(CMAKE_C_SIZEOF_DATA_PTR 4 CACHE STRING "" FORCE)
set(CMAKE_CXX_SIZEOF_DATA_PTR 4 CACHE STRING "" FORCE)

# Allow finding both host and target files — we're cross-compiling via
# Wine wrappers but all files are on the local filesystem.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
