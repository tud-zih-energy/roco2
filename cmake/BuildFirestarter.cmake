
FILE(GLOB_RECURSE FS_SRC_FILES lib/FIRESTARTER/*)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/firestarter
    COMMAND cmake -E make_directory ${CMAKE_BINARY_DIR}/firestarter
    COMMENT "Create FIRESTARTER source code directory"
    VERBATIM
)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/firestarter/Makefile
    COMMAND ${CMAKE_SOURCE_DIR}/lib/FIRESTARTER/code-generator.py -v
    DEPENDS ${FS_SRC_FILES} ${CMAKE_BINARY_DIR}/firestarter
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/firestarter
    COMMENT "Generate FIRESTARTER source code"
    VERBATIM
)

add_custom_target(GenerateFirestarterSource ALL DEPENDS ${CMAKE_BINARY_DIR}/firestarter/Makefile)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/firestarter/libfirestarter.a
    COMMAND make libfirestarter.a
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/firestarter
    COMMENT "Build libfirestarter.a"
    VERBATIM
)

add_custom_target(libfirestarter ALL DEPENDS ${CMAKE_BINARY_DIR}/firestarter/libfirestarter.a)

set(FIRESTARTER_LIBRARIES ${CMAKE_BINARY_DIR}/firestarter/libfirestarter.a)
set(FIRESTARTER_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/firestarter)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/firestarter)