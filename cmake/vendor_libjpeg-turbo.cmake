include(ExternalProject)
set(LIBJPEG_TURBO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/libjpeg-turbo")
set(LIBJPEG_TURBO_PREFIX "${CMAKE_BINARY_DIR}/vendor_libjpeg-turbo")
set(LIBJPEG_TURBO_INSTALL_PREFIX "${LIBJPEG_TURBO_PREFIX}/install/$<CONFIG>")
set(LIBJPEG_ARGS
    "-DCMAKE_INSTALL_PREFIX=${LIBJPEG_TURBO_INSTALL_PREFIX}")
if(MSVC)
    list(APPEND LIBJPEG_ARGS 
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
        "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} /MP"
        "-DCMAKE_C_FLAGS=/MP")
endif()
ExternalProject_Add(
    libjpeg-turbo_external
    PREFIX ${LIBJPEG_TURBO_PREFIX}
    SOURCE_DIR ${LIBJPEG_TURBO_SOURCE_DIR}
    CMAKE_ARGS ${LIBJPEG_ARGS}
)
set_target_properties(libjpeg-turbo_external PROPERTIES FOLDER "Vendor")
add_library(libjpeg-turbo INTERFACE)
target_link_libraries(libjpeg-turbo INTERFACE "${LIBJPEG_TURBO_INSTALL_PREFIX}/lib/turbojpeg-static.lib")
target_include_directories(libjpeg-turbo INTERFACE "${LIBJPEG_TURBO_INSTALL_PREFIX}/include")
add_dependencies(libjpeg-turbo libjpeg-turbo_external)
