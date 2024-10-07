cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

if (MSVC_TOOLSET_VERSION)
    message("-- MSVC_TOOLSET_VERSION: ${MSVC_TOOLSET_VERSION}")
endif()

project(GTASA C CXX)

if (MSVC)
    add_compile_options("/bigobj")
    add_compile_options("/utf-8")
endif()

option(BUILD_DOC "Build documentation" OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(Documentation)

set(RE_PROJECT GTASA)
set(RE_PROJECT_LIB_NAME gta_reversed)
set(RE_PROJECT_APP_NAME gta_reversed_app)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT ${RE_PROJECT_LIB_NAME}
)

# Put DLLs in root
set(CMAKE_INSTALL_BINDIR .)
# And other components in its own folder
set(CMAKE_INSTALL_LIBDIR ${RE_PROJECT_LIB_NAME}/lib)
set(CMAKE_INSTALL_INCLUDEDIR ${RE_PROJECT_LIB_NAME}/include)

add_subdirectory(libs/ EXCLUDE_FROM_ALL SYSTEM)
add_subdirectory(source)

function(create_source_group target)
    get_property(sources TARGET ${target} PROPERTY SOURCES)
    foreach (filename ${sources})
        source_group("${path}" FILES ${filename})
    endforeach ()
endfunction()
create_source_group(${RE_PROJECT_LIB_NAME})

