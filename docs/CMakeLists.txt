option(DBPP_ENABLE_DOCS "Generate documentation using Doxygen" OFF)

if (DBPP_ENABLE_DOCS)
    find_package(Doxygen)
    if (NOT DOXYGEN_FOUND)
        message(WARNING "Can't generate documentation, since doxygen was not found!")
        return()
    endif()

    list(APPEND DOXYGEN_ALIASES "includeexample{1}=\\par Example^^ \\includelineno \\1")
    list(APPEND DOXYGEN_ALIASES "includeexample{2}=\\par Example^^ \\1 \\n \\includelineno \\2")
    list(APPEND DOXYGEN_ALIASES "includeexamplewithoutput{1}=\\par Example^^ \\includelineno \\1 \\par Output^^ \\verbinclude \\1.output")
    list(APPEND DOXYGEN_ALIASES "includeexamplewithoutput{2}=\\par Example^^ \\1 \\n \\includelineno \\2 \\par Output^^ \\verbinclude \\1.output")
    list(APPEND DOXYGEN_EXAMPLE_PATH "${CMAKE_CURRENT_BINARY_DIR}/examples")
    list(APPEND DOXYGEN_EXCLUDE_SYMBOLS "detail")
    list(APPEND DOXYGEN_EXCLUDE_SYMBOLS "Detail")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE mainpage.md)
    set(DOXYGEN_PREDEFINED DOXYGEN_SHOULD_SEE_THIS)

    doxygen_add_docs(docs mainpage.md ${DBPP_DOXYGEN_INPUT_DIRS} ALL)

    set(EXAMPLES
        examples/ConnectionClass.cpp
        examples/ConnectionPrepare.cpp
        examples/ReadmeExample.cpp
    )

    foreach(EXAMPLE ${EXAMPLES})
        get_filename_component(EXECUTABLE ${EXAMPLE} NAME_WE)
        add_executable(${EXECUTABLE} ${EXAMPLE})
        target_link_libraries(${EXECUTABLE} PRIVATE dbpp::Sqlite3)
        configure_file(${EXAMPLE} ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE} COPYONLY)

        add_custom_command(OUTPUT ${EXAMPLE}.output
            COMMAND ${CMAKE_COMMAND} -DCOMMAND=$<TARGET_FILE:${EXECUTABLE}> -DOUTPUT_FILE=${EXAMPLE}.output -P ${PROJECT_SOURCE_DIR}/cmake/ExecuteAndCaptureStdout.cmake
            DEPENDS ${EXECUTABLE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        list(APPEND EXAMPLE_OUTPUT_FILES ${EXAMPLE}.output)
    endforeach()

    add_custom_target(example_outputs DEPENDS ${EXAMPLE_OUTPUT_FILES})
    add_dependencies(docs example_outputs)

    if (DBPP_ENABLE_INSTALL)
        install(
            DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
            DESTINATION ${CMAKE_INSTALL_DOCDIR}
            COMPONENT dbpp_Docs
        )
    endif()
endif()
