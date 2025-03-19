option(${RE_PROJECT}_UNITY_BUILD "Use Unity/Jumbo build" OFF)
option(${RE_PROJECT}_WITH_SANITIZERS "Use Sanitizers (better crash log)" OFF)
option(${RE_PROJECT}_WITH_LTO "Use Link Time Optimization" OFF)
set(${RE_PROJECT}_GAME_DIRECTORY "" CACHE STRING "Copy to directory")

file(
    GLOB_RECURSE SOURCE_FILES_LIST
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h*"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c*"
)

foreach(FILE ${SOURCE_FILES_LIST})
    # Get the relative path of the file with respect to the base directory
    file(RELATIVE_PATH FILE_RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${FILE}")
    
    # Extract the directory part of the relative path
    get_filename_component(FILE_DIRECTTORY "${FILE_RELATIVE}" DIRECTORY)
    
    # Use the directory as the group name, or fall back to prefix if empty
    if(FILE_DIRECTTORY)
        string(REPLACE "/" "\\" FILE_GROUP "${FILE_DIRECTTORY}")
        source_group("${FILE_GROUP}" FILES "${FILE}")
    else()
        source_group("" FILES "${FILE}")
    endif()
endforeach()

add_library(${RE_PROJECT_LIB_NAME} MODULE ${SOURCE_FILES_LIST})

target_compile_options(${RE_PROJECT_LIB_NAME} PRIVATE
    "$<$<CXX_COMPILER_ID:MSVC>:/MP;/Zc:preprocessor;/wd26812;/wd26495>"
    "$<$<OR:$<CXX_COMPILER_ID:Clang,GCC>,$<CXX_COMPILER_ID:GNU>>:-Wall;>"
)

function(supress_deps_warnings targets)
    foreach (target ${targets})
        add_compile_options(${target} PRIVATE
            "$<$<CXX_COMPILER_ID:MSVC>:-w>"
            "$<$<OR:$<CXX_COMPILER_ID:Clang,GCC>,$<CXX_COMPILER_ID:GNU>>:-Wno-everything>"
        )
    endforeach()
endfunction()
supress_deps_warnings(ogg imgui vorbisfile spdlog nlohmann_json)

set(GAME_DIRECTORY "${${RE_PROJECT}_GAME_DIRECTORY}")
if("${GAME_DIRECTORY}" STREQUAL "")
    if(EXISTS "${PROJECT_SOURCE_DIR}/game/gta_sa.exe")
        file(SIZE "${PROJECT_SOURCE_DIR}/game/gta_sa.exe" FILE_SIZE)
        if(NOT FILE_SIZE EQUAL 5189632)
            message(FATAL_ERROR "Error: gta_sa.exe size is not exactly 5,189,632 bytes!")
        else()
            message(STATUS "Found valid compact executable! '${PROJECT_SOURCE_DIR}/game/gta_sa.exe'")
        endif()
        set(GAME_DIRECTORY "${PROJECT_SOURCE_DIR}/game")
    endif()
endif()
if(NOT "${GAME_DIRECTORY}" STREQUAL "")
    set(OUTPUT_DIRECTORY "${GAME_DIRECTORY}/scripts")
    set_target_properties(${RE_PROJECT_LIB_NAME} PROPERTIES VS_DEBUGGER_COMMAND "${GAME_DIRECTORY}/gta_sa.exe")
    set_target_properties(${RE_PROJECT_LIB_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${GAME_DIRECTORY}")
elseif(MSVC)
    set(OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIG>")
else()
    set(OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
endif()

set_target_properties(${RE_PROJECT_LIB_NAME}
    PROPERTIES
    SUFFIX ".asi"
    ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIRECTORY}"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIRECTORY}"
    COMPILE_WARNING_AS_ERROR ON
)

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

target_link_options(${RE_PROJECT_LIB_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")

add_dependencies(${RE_PROJECT_LIB_NAME} ogg)
target_link_libraries(${RE_PROJECT_LIB_NAME} PRIVATE
    ogg
    vorbis
    imgui
    spdlog
    tracy
    nlohmann_json
    libjpeg-turbo
    ddraw
    Winmm
    dxguid
    strmiids
    dsound
    d3d9
)

if(WIN32) 
    if(MSVC) # MSVC knows how to find dbghelp on it's own
        target_link_libraries(${RE_PROJECT_LIB_NAME} PRIVATE
            dbghelp
        )
    else()
        message(FATAL_ERROR "Need to link dbghelp lib for win32")
    endif()
endif()

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
target_compile_definitions(${RE_PROJECT_LIB_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
    USE_GTASA_ALLOCATOR
    EXTRA_DEBUG_FEATURES
    FIX_BUGS
    SA_SKINNED_PEDS
)
target_compile_features(${RE_PROJECT_LIB_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${RE_PROJECT_LIB_NAME} PRIVATE StdInc.cpp)

#--------------------------------------------------------------------
# Install
#--------------------------------------------------------------------
if (${RE_PROJECT}_INSTALL)
    install(
        TARGETS ${RE_PROJECT_LIB_NAME}
        EXPORT ${RE_PROJECT_LIB_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
    if (MSVC)
        install(FILES $<TARGET_PDB_FILE:${RE_PROJECT_LIB_NAME}> DESTINATION ${CMAKE_INSTALL_PREFIX} OPTIONAL)
    endif()
endif()
