# Find Doxygen once before overriding find_package
find_package(Doxygen REQUIRED)

# Override find_package to mock everything except Doxygen
macro(find_package name)
    if("${name}" STREQUAL "Doxygen")
        # No-op for Doxygen: preserve the result of the original find_package(Doxygen)
    else()
        message(STATUS "Mocking find_package for ${name}")
        set(${name}_FOUND TRUE)
        set(${name}_DIR "")
    endif()
endmacro()

# Provide dummy interface targets so target_link_libraries() doesn't fail
add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED)
add_library(Ogg::ogg INTERFACE IMPORTED)
add_library(spdlog::spdlog INTERFACE IMPORTED)
add_library(Tracy::TracyClient INTERFACE IMPORTED)
add_library(vorbis::vorbis INTERFACE IMPORTED)
add_library(imgui::imgui INTERFACE IMPORTED)
add_library(libjpeg-turbo::libjpeg-turbo INTERFACE IMPORTED)
add_library(SDL3::SDL3 INTERFACE IMPORTED)

