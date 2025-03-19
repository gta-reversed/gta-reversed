include(ExternalProject)
set(NLOHMANN_JSON_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor/nlohmann_json")
set(NLOHMANN_JSON_PREFIX "${CMAKE_BINARY_DIR}/vendor_nlohmann_json")
set(NLOHMANN_JSON_INSTALL_PREFIX "${NLOHMANN_JSON_PREFIX}/install/$<CONFIG>")
set(NLOHMANN_JSON_ARGS
        "-DCMAKE_INSTALL_PREFIX=${NLOHMANN_JSON_INSTALL_PREFIX}"
        "-DJSON_BuildTests=OFF")
if(MSVC)
    list(APPEND NLOHMANN_JSON_ARGS 
        "-DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}")
endif()
ExternalProject_Add(
    nlohmann_json_external
    PREFIX ${NLOHMANN_JSON_PREFIX}
    SOURCE_DIR ${NLOHMANN_JSON_SOURCE_DIR}
    CMAKE_ARGS ${NLOHMANN_JSON_ARGS}
)
set_target_properties(nlohmann_json_external PROPERTIES FOLDER "Vendor")
add_library(nlohmann_json INTERFACE)
#target_link_libraries(nlohmann_json INTERFACE "${NLOHMANN_JSON_INSTALL_PREFIX}/lib/json.lib")
target_include_directories(nlohmann_json INTERFACE "${NLOHMANN_JSON_INSTALL_PREFIX}/include")
add_dependencies(nlohmann_json nlohmann_json_external)
