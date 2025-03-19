include(ExternalProject)
set(VORBIS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/vorbis")
set(VORBIS_PREFIX "${CMAKE_BINARY_DIR}/vendor_vorbis")
set(VORBIS_INSTALL_PREFIX "${VORBIS_PREFIX}/install/$<CONFIG>")
set(VORBIS_ARGS
        "-DCMAKE_INSTALL_PREFIX=${VORBIS_INSTALL_PREFIX}"
        "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
        "-DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}"
        "-DCMAKE_CXX_FLAGS_MINSIZEREL=${CMAKE_CXX_FLAGS_MINSIZEREL}"
        "-DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}"
        "-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
        "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
        "-DCMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}"
        "-DCMAKE_C_FLAGS_MINSIZEREL=${CMAKE_C_FLAGS_MINSIZEREL}"
        "-DCMAKE_C_FLAGS_RELEASE=${CMAKE_C_FLAGS_RELEASE}"
        "-DCMAKE_C_FLAGS_RELWITHDEBINFO=${CMAKE_C_FLAGS_RELWITHDEBINFO}"
        "-DOGG_INCLUDE_DIR=${OGG_INSTALL_PREFIX}/include"
        "-DOGG_LIBRARY=${OGG_INSTALL_PREFIX}/lib/ogg.lib"
        "-DBUILD_TESTING=OFF")
if(MSVC)
    list(APPEND VORBIS_ARGS 
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}"
        "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} /MP"
        "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS} /MP")
endif()
ExternalProject_Add(
    vorbis_external
    PREFIX ${VORBIS_PREFIX}
    SOURCE_DIR ${VORBIS_SOURCE_DIR}
    CMAKE_ARGS ${VORBIS_ARGS}
)
add_dependencies(vorbis_external ogg_external)
set_target_properties(vorbis_external PROPERTIES FOLDER "Vendor")
add_library(vorbis INTERFACE)
target_link_libraries(vorbis INTERFACE "${VORBIS_INSTALL_PREFIX}/lib/vorbis.lib")
target_link_libraries(vorbis INTERFACE "${VORBIS_INSTALL_PREFIX}/lib/vorbisfile.lib")
target_link_libraries(vorbis INTERFACE "${VORBIS_INSTALL_PREFIX}/lib/vorbisenc.lib")
target_link_libraries(vorbis INTERFACE ogg)
target_include_directories(vorbis INTERFACE "${VORBIS_INSTALL_PREFIX}/include")
add_dependencies(vorbis vorbis_external)
