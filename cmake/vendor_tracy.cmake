include(ExternalProject)
set(TRACY_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/tracy")
set(TRACY_PREFIX "${CMAKE_BINARY_DIR}/vendor_tracy")
set(TRACY_INSTALL_PREFIX "${TRACY_PREFIX}/install/$<CONFIG>")
set(TRACY_ARGS
        "-DCMAKE_INSTALL_PREFIX=${TRACY_INSTALL_PREFIX}")
if(MSVC)
    list(APPEND TRACY_ARGS 
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()
ExternalProject_Add(
    tracy_external
    PREFIX ${TRACY_PREFIX}
    SOURCE_DIR ${TRACY_SOURCE_DIR}
    CMAKE_ARGS ${TRACY_ARGS}
)
set_target_properties(tracy_external PROPERTIES FOLDER "Vendor")
add_library(tracy INTERFACE)
target_link_libraries(tracy INTERFACE "${TRACY_INSTALL_PREFIX}/lib/TracyClient.lib")
target_include_directories(tracy INTERFACE "${TRACY_INSTALL_PREFIX}/include")
target_include_directories(tracy INTERFACE "${TRACY_INSTALL_PREFIX}/include/tracy/tracy")
add_dependencies(tracy tracy_external)
