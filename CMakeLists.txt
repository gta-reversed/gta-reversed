cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC AND NOT CMAKE_GENERATOR MATCHES "Ninja")
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC_TOOLSET_VERSION)
    message("-- MSVC_TOOLSET_VERSION: ${MSVC_TOOLSET_VERSION}")
endif()

project(GTASA C CXX)
set(RE_PROJECT GTASA)
set(RE_PROJECT_LIB_NAME gta_reversed)
set(RE_PROJECT_APP_NAME gta_reversed_app)

option(${RE_PROJECT}_NO_EDIT_AND_CONTINUE "Disable Edit&Continue" OFF)

if (MSVC)
    add_compile_options(/bigobj /utf-8)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    if (${RE_PROJECT}_NO_EDIT_AND_CONTINUE)
        add_compile_options(/Z7 /FS)
        add_link_options(/MAP) # /SAFESEH:NO disabled, /MAP added for Wine build
    else()
        # For Edit&Continue
        add_compile_options(/ZI)
        add_link_options(/SAFESEH:NO)
    endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
include(Documentation)

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

add_subdirectory(source)

function(create_source_group target)
    get_property(sources TARGET ${target} PROPERTY SOURCES)
    foreach (filename ${sources})
        source_group("${path}" FILES ${filename})
    endforeach ()
endfunction()
create_source_group(${RE_PROJECT_LIB_NAME})

