cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(GTASA C CXX)
option(BUILD_DOC "Build documentation" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
if (MSVC)
    add_compile_options(/bigobj /utf-8)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # For Edit & Continue
    add_compile_options("$<$<CONFIG:Debug>:/ZI>")
    add_link_options("$<$<CONFIG:Debug>:/SAFESEH:NO>")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/vendor/")
if(BUILD_DOC)
    include(Documentation)
endif()
include(vendor_ogg)
include(vendor_vorbis)
include(vendor_spdlog)
include(vendor_nlohmann_json)
include(vendor_tracy)
include(vendor_imgui)
include(vendor_libjpeg-turbo)

set(RE_PROJECT GTASA)
set(RE_PROJECT_LIB_NAME gta_reversed)
set(RE_PROJECT_APP_NAME gta_reversed_app)

add_subdirectory(source)
