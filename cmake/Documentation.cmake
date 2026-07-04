# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY doxygen)
    set(DOXYGEN_COLLABORATION_GRAPH YES)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_CLASS_DIAGRAMS YES)
    set(DOXYGEN_HIDE_UNDOC_RELATIONS NO)
    set(DOXYGEN_HAVE_DOT NO)
    set(DOXYGEN_CLASS_GRAPH YES)
    set(DOXYGEN_CALL_GRAPH YES)
    set(DOXYGEN_CALLER_GRAPH YES)
    set(DOXYGEN_COLLABORATION_GRAPH YES)
    set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_PACKAGE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    set(DOXYGEN_EXTRACT_LOCALMETHODS YES)
    set(DOXYGEN_UML_LOOK YES)
    set(DOXYGEN_UML_LIMIT_NUM_FIELDS 50)
    set(DOXYGEN_TEMPLATE_RELATIONS YES)
    set(DOXYGEN_DOT_GRAPH_MAX_NODES 100)
    set(DOXYGEN_MAX_DOT_GRAPH_DEPTH 0)
    set(DOXYGEN_DOT_TRANSPARENT YES)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${PROJECT_SOURCE_DIR}/README.md")

    set(DOXYGEN_ALIASES
        "notsa=\\xrefitem notsa \\\"NOTSA\\\" \\\"NOTSA\\\" "
        "unused=\\xrefitem unused \\\"Unused\\\" \\\"Unused\\\" "
        "addr=\\par Memory address: "
        "size=\\par Structure size: "
    )

    doxygen_add_docs(doxygen
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${PROJECT_SOURCE_DIR}/docs
        ${PROJECT_SOURCE_DIR}/README.md
    )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
