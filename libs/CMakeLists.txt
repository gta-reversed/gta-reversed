set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(BUILD_SHARED_LIBS ON)
set(INSTALL_DOCS OFF)
set(INSTALL_PKG_CONFIG_MODULE OFF)
set(INSTALL_CMAKE_PACKAGE_MODULE OFF)
set(OGG_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/ogg/include")
set(OGG_LIBRARY ogg)
add_subdirectory(ogg EXCLUDE_FROM_ALL)

add_subdirectory(vorbis EXCLUDE_FROM_ALL)

set(SPDLOG_ENABLE_PCH ON)
set(SPDLOG_USE_STD_FORMAT ON)
add_subdirectory(spdlog EXCLUDE_FROM_ALL)

add_subdirectory(json EXCLUDE_FROM_ALL)

#############################################################################################
############# create the imgui project
#############################################################################################

set(IMGUI_DIR "imgui")
add_library(imgui STATIC)

target_sources(imgui PRIVATE
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp

    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx9.cpp

    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/misc/
    ${IMGUI_DIR}/misc/cpp
    ${IMGUI_DIR}/misc/debuggers
)
target_link_libraries(imgui PRIVATE d3d9 ${CMAKE_DL_LIBS})
target_compile_definitions(imgui PRIVATE _CRT_SECURE_NO_WARNINGS)

# set options before add_subdirectory
# available options: TRACY_ENABLE , TRACY_ON_DEMAND , TRACY_NO_BROADCAST , TRACY_NO_CODE_TRANSFER , ...
set(TRACY_ENABLE ON)
set(TRACY_ON_DEMAND ON)
set(TRACY_CALLSTACK ON)
add_subdirectory(tracy) # target: TracyClient or alias Tracy :: TracyClient
