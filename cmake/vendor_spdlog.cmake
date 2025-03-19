include(ExternalProject)
set(SPDLOG_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/spdlog")
set(SPDLOG_PREFIX "${CMAKE_BINARY_DIR}/vendor_spdlog")
set(SPDLOG_INSTALL_PREFIX "${SPDLOG_PREFIX}/install/$<CONFIG>")
set(SPDLOG_ARGS
        "-DCMAKE_INSTALL_PREFIX=${SPDLOG_INSTALL_PREFIX}"
        "-DSPDLOG_BUILD_EXAMPLE=OFF")
if(MSVC)
    list(APPEND SPDLOG_ARGS 
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()
ExternalProject_Add(
    spdlog_external
    PREFIX ${SPDLOG_PREFIX}
    SOURCE_DIR ${SPDLOG_SOURCE_DIR}
    CMAKE_ARGS ${SPDLOG_ARGS}
)
set_target_properties(spdlog_external PROPERTIES FOLDER "Vendor")
add_library(spdlog INTERFACE)
target_link_libraries(spdlog INTERFACE "${SPDLOG_INSTALL_PREFIX}/lib/spdlog$<$<CONFIG:Debug>:d>.lib")
target_include_directories(spdlog INTERFACE "${SPDLOG_INSTALL_PREFIX}/include")
add_dependencies(spdlog spdlog_external)
