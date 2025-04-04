#--------------------------------------------------------------------
# Game features
#--------------------------------------------------------------------
option(${RE_PROJECT}_WITH_OPENAL "Use OpenAL" OFF)


#--------------------------------------------------------------------
# Build options
#--------------------------------------------------------------------
option(${RE_PROJECT}_UNITY_BUILD "Use Unity/Jumbo build" OFF)
option(${RE_PROJECT}_WITH_SANITIZERS "Use Sanitizers (better crash log)" OFF)
option(${RE_PROJECT}_WITH_LTO "Use Link Time Optimization" OFF)
option(${RE_PROJECT}_WITH_SCRIPT_COMMAND_HOOKS "Create script command hooks in the UI (Command hooks are installed regardless)" OFF)
option(${RE_PROJECT}_WITH_CLEO_COMMANDS "Enable CLEO script commands" OFF)

file(
    GLOB_RECURSE SOURCE_FILES_LIST
    LIST_DIRECTORIES false
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h*"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"
)
list(APPEND SOURCE_FILES_LIST StdInc.cpp)

#--------------------------------------------------------------------
# Remove unused sources
#--------------------------------------------------------------------
if (NOT WIN32 AND NOT ${RE_PROJECT}_WITH_OPENAL)
    set(${RE_PROJECT}_WITH_OPENAL ON)
endif()

if (${RE_PROJECT}_WITH_OPENAL)
    # TODO: Remove dsound specific sources
else()
    file(
        GLOB_RECURSE OPENAL_FILES
        LIST_DIRECTORIES false
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/game_sa/Audio/OpenAL/*.c*"
        "${CMAKE_CURRENT_SOURCE_DIR}/game_sa/Audio/OpenAL/*.h*"
    )
    list(REMOVE_ITEM SOURCE_FILES_LIST ${OPENAL_FILES})
endif()


add_library(${RE_PROJECT_LIB_NAME} MODULE ${SOURCE_FILES_LIST})

target_compile_options(${RE_PROJECT_LIB_NAME} PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/MP;/Zc:preprocessor;/W3;/wd4251;/wd4275;/wd4996;/wd26495;/wd26812;/wd28159>"
    "$<$<OR:$<CXX_COMPILER_ID:Clang,GCC>,$<CXX_COMPILER_ID:GNU>>:-Wall;>"
)
set_target_properties(${RE_PROJECT_LIB_NAME}
    PROPERTIES
    SUFFIX ".asi"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/"
    COMPILE_WARNING_AS_ERROR ON
)
target_link_options(${RE_PROJECT_LIB_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")

#--------------------------------------------------------------------
# Add dependencies
#--------------------------------------------------------------------
macro(add_dependency)
    foreach (target ${ARGV})
        find_package(${target} REQUIRED)

        add_compile_options(${target} PRIVATE
            "$<$<CXX_COMPILER_ID:MSVC>:-w>"
            "$<$<OR:$<CXX_COMPILER_ID:Clang,GCC>,$<CXX_COMPILER_ID:GNU>>:-Wno-everything>"
        )
    endforeach()
endmacro()
add_dependency(
    nlohmann_json
    Ogg
    spdlog
    Tracy
    Vorbis
    imgui
    libjpeg-turbo
)

target_link_libraries(${RE_PROJECT_LIB_NAME} PRIVATE
    nlohmann_json::nlohmann_json
    Ogg::ogg
    spdlog::spdlog
    Tracy::TracyClient
    vorbis::vorbis
    imgui::imgui
    libjpeg-turbo::libjpeg-turbo
    ddraw
    Winmm
    dxguid
    strmiids
    d3d9
)


target_include_directories(${RE_PROJECT_LIB_NAME} PRIVATE
    /
    toolsmenu/
    game_sa/
    game_sa/Debug/
    game_sa/Animation/
    game_sa/Attractors/

    game_sa/Audio/
    game_sa/Audio/Config
    game_sa/Audio/Entities
    game_sa/Audio/Hardware
    game_sa/Audio/Loaders
    game_sa/Audio/Managers
    game_sa/Audio/Utility

    game_sa/Collision/
    game_sa/Core/
    game_sa/Entity/
    game_sa/Entity/Dummy/
    game_sa/Entity/Object/
    game_sa/Entity/Ped/
    game_sa/Entity/Vehicle/
    game_sa/Enums/
    game_sa/Events/
    game_sa/Frontend/
    game_sa/Fx/
    game_sa/Fx/Blueprints/
    game_sa/Fx/Blueprints/Interp
    game_sa/Interior/
    game_sa/Memory/
    game_sa/Models/
    game_sa/Plant/
    game_sa/Plugins/
    game_sa/Pipelines/
    game_sa/Pipelines/CustomBuilding
    game_sa/Pipelines/CustomCarEnvMap
    game_sa/RenderWare/
    game_sa/RenderWare/rw/
    game_sa/Scripts/
    game_sa/Tasks/
    game_sa/Tasks/TaskTypes/
    game_sa/Tasks/TaskTypes/SeekEntity/

    app/
    app/platform/
    app/platform/android/
    app/platform/win/
    app/platform/win/VideoPlayer/
    app/platform/linux/
    app/securom/

    oswrapper
)

target_sources(${RE_PROJECT_LIB_NAME} PRIVATE
    notsa.natvis
)

target_compile_definitions(${RE_PROJECT_LIB_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
    USE_GTASA_ALLOCATOR
    EXTRA_DEBUG_FEATURES
    FIX_BUGS
    SA_SKINNED_PEDS

    USE_D3D9 # TODO
)
target_compile_features(${RE_PROJECT_LIB_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${RE_PROJECT_LIB_NAME} PRIVATE StdInc.cpp)

#--------------------------------------------------------------------
# Configure build options
#--------------------------------------------------------------------
if (${RE_PROJECT}_UNITY_BUILD)
    set_target_properties(${RE_PROJECT_LIB_NAME}
        PROPERTIES
        UNITY_BUILD TRUE
        UNITY_BUILD_MODE BATCH
        UNITY_BUILD_BATCH_SIZE 16
    )
endif()

if (${RE_PROJECT}_WITH_LTO)
    message(STATUS "Configuring Link Time Optimization")
    set_target_properties(${RE_PROJECT_LIB_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (${RE_PROJECT}_WITH_SANITIZERS)
    message(STATUS "Configuring sanitizers")
    # add_sanitizers(${RE_PROJECT_LIB_NAME})
endif()


#--------------------------------------------------------------------
# Configure the Audio API
#--------------------------------------------------------------------
if (${RE_PROJECT}_WITH_OPENAL)
    target_compile_definitions(${RE_PROJECT_LIB_NAME} PRIVATE USE_OPENAL)

    target_include_directories(${RE_PROJECT_LIB_NAME} PRIVATE
        game_sa/Audio/OpenAL
    )
else()
    target_compile_definitions(${RE_PROJECT_LIB_NAME} PRIVATE USE_DSOUND)

    target_link_libraries(${RE_PROJECT_LIB_NAME} PRIVATE dsound)
endif()


#--------------------------------------------------------------------
# Configure misc. dependencies for Windows
#--------------------------------------------------------------------
if(WIN32)
    if(MSVC)
        # MSVC knows how to find dbghelp on it's own
        target_link_libraries(${RE_PROJECT_LIB_NAME} PRIVATE
            dbghelp
        )

        target_compile_definitions(${RE_PROJECT_LIB_NAME} PRIVATE DIRECTINPUT_VERSION=0x0800)
    else()
        message(FATAL_ERROR "Need to link dbghelp lib for win32")
    endif()
endif()


#--------------------------------------------------------------------
# Install
#--------------------------------------------------------------------
if (${RE_PROJECT}_INSTALL)
    install(
        TARGETS ${RE_PROJECT_LIB_NAME}
        EXPORT ${RE_PROJECT_LIB_NAME}-targets
        RUNTIME DESTINATION "."
    )
    if (MSVC)
        install(FILES $<TARGET_PDB_FILE:${RE_PROJECT_LIB_NAME}> DESTINATION "." OPTIONAL)
    endif()
endif()
