if (SCOREP_LIBRARIES AND SCOREP_INCLUDE_DIRS)
  set (Scorep_FIND_QUIETLY TRUE)
endif ()

IF(SCOREP_CONFIG_PATH)
    FIND_PROGRAM(SCOREP_CONFIG NAMES scorep-config
        PATHS ${SCOREP_CONFIG_PATH} /opt/scorep
        PATH_SUFFIXES bin
    )
ELSE(SCOREP_CONFIG_PATH)
    FIND_PROGRAM(SCOREP_CONFIG NAMES scorep-config
        PATHS /opt/scorep ENV PATH
        PATH_SUFFIXES bin
    )
ENDIF(SCOREP_CONFIG_PATH)

IF(NOT SCOREP_CONFIG)
    macro(add_scorep_instrumentation tgt)
    endmacro()
ELSE(NOT SCOREP_CONFIG)

    if(NOT SCOREP_CONFIG STREQUAL OLD_SCOREP_CONFIG)
        set(OLD_SCOREP_CONFIG ${SCOREP_CONFIG} CACHE INTERNAL "previous value of SCOREP_CONFIG")
        unset(SCOREP_INCLUDE_DIRS CACHE)
        unset(SCOREP_EXECUTABLE CACHE)
    endif()

    execute_process(COMMAND ${SCOREP_CONFIG} "--cppflags" OUTPUT_VARIABLE SCOREP_INCLUDE_DIRS)
    STRING(REPLACE "-I" ";" SCOREP_INCLUDE_DIRS "${SCOREP_INCLUDE_DIRS}")

    execute_process(COMMAND ${SCOREP_CONFIG} "--ldflags" OUTPUT_VARIABLE _LINK_LD_ARGS)
    STRING( REPLACE " " ";" _LINK_LD_ARGS "${_LINK_LD_ARGS}" )
    FOREACH( _ARG ${_LINK_LD_ARGS} )
        IF(${_ARG} MATCHES "^-L")
            STRING(REGEX REPLACE "^-L" "" _ARG "${_ARG}")
            SET(SCOREP_LINK_DIRS ${SCOREP_LINK_DIRS} ${_ARG})
        ENDIF(${_ARG} MATCHES "^-L")
    ENDFOREACH(_ARG)

    execute_process(COMMAND ${SCOREP_CONFIG} "--libs" OUTPUT_VARIABLE _LINK_LD_ARGS)
    STRING( REPLACE " " ";" _LINK_LD_ARGS "${_LINK_LD_ARGS}" )
    FOREACH( _ARG ${_LINK_LD_ARGS} )
        IF(${_ARG} MATCHES "^-l")
            STRING(REGEX REPLACE "^-l" "" _ARG ${_ARG})
            FIND_LIBRARY(_SCOREP_LIB_FROM_ARG NAMES ${_ARG}
                PATHS
                ${SCOREP_LINK_DIRS}
            )
            IF(_SCOREP_LIB_FROM_ARG)
                SET(SCOREP_LIBRARIES ${SCOREP_LIBRARIES} ${_SCOREP_LIB_FROM_ARG})
            ENDIF(_SCOREP_LIB_FROM_ARG)
            UNSET(_SCOREP_LIB_FROM_ARG CACHE)
        ENDIF(${_ARG} MATCHES "^-l")
    ENDFOREACH(_ARG)


    FIND_PROGRAM(SCOREP_EXECUTABLE NAMES scorep
        PATHS /opt/scorep ENV PATH
        PATH_SUFFIXES bin
    )

    option(SCOREP_COMPILER_INSTRUMENTATION "Enable Score-P compiler instrumentation" OFF)
    option(SCOREP_USER_INSTRUMENTATION "Enable Score-P user instrumentation" ON)

    list(APPEND SCOREP_EXECUTABLE_FLAGS "--dynamic" "--openmp")

    if(SCOREP_COMPILER_INSTRUMENTATION)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--compiler")
    else()
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--nocompiler")
    endif()

    if(SCOREP_USER_INSTRUMENTATION)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--user")
    else()
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--nouser")
    endif()

    macro(add_scorep_instrumentation tgt)

        get_target_property(TGT_INC ${tgt} "INCLUDE_DIRECTORIES")

        set(INC_DIRS)
        foreach(INC_DIR ${TGT_INC})
            string(STRIP "${INC_DIR}" INC_DIR)
            list(APPEND INC_DIRS "-I${INC_DIR}")
        endforeach()

        get_target_property(TGT_SRCS ${tgt} "SOURCES")

        get_target_property(TGT_LIBS ${tgt} "LINK_LIBRARIES")
        set(TGT_LINK_LIBS)
        foreach(TGT_LIB ${TGT_LIBS})
            string(STRIP "${TGT_LIB}" TGT_LIB)

            get_filename_component(LIB_DIR ${TGT_LIB} DIRECTORY)
            get_filename_component(LIB_NAME ${TGT_LIB} NAME_WE)
            get_filename_component(LIB_EXT ${TGT_LIB} EXT)

            if(LIB_EXT STREQUAL "")
                if(LIB_DIR STREQUAL "")
                    get_target_property(TGT_LIB ${TGT_LIB} "LOCATION")

                    get_filename_component(LIB_DIR ${TGT_LIB} DIRECTORY)
                    get_filename_component(LIB_NAME ${TGT_LIB} NAME_WE)
                endif()
            endif()

            string(REPLACE "lib" "" LIB_NAME ${LIB_NAME})

            #list(APPEND TGT_LINK_LIBS "-L${LIB_DIR}")
            #list(APPEND TGT_LINK_LIBS "-l${LIB_NAME}")
            list(APPEND TGT_LINK_LIBS "${TGT_LIB}")
        endforeach()

        get_directory_property(TGT_DEFS "COMPILE_DEFINITIONS")
        set(TGT_DEFINITIONS)
        foreach(TGT_DEF ${TGT_DEFS})
            string(STRIP "${TGT_DEF}" TGT_DEF)
            list(APPEND TGT_DEFINITIONS "-D${TGT_DEF}")
        endforeach()

        string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

        set(TGT_LINK_FILES)
        foreach(SRC ${TGT_SRCS})
            get_source_file_property(SRC_LANG ${SRC} "LANGUAGE")
            get_filename_component(SRC_NAME ${SRC} NAME_WE)
            get_filename_component(SRC_DIR ${SRC} DIRECTORY)

            if(${SRC_LANG} STREQUAL "CXX")
                set(SRC_LANG_LOWER "cpp")
            else()
                set(SRC_LANG_LOWER "c")
            endif()

            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${tgt}_scorep.dir/${SRC_DIR})

            set(SRC_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${tgt}_scorep.dir/${SRC_DIR}/${SRC_NAME}.${SRC_LANG_LOWER}.o")
            set(SRC_PLAIN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${tgt}.dir/${SRC_DIR}/${SRC_NAME}.${SRC_LANG_LOWER}.o")

            string(REPLACE "${CMAKE_BINARY_DIR}/" "" SRC_SHORT_OUTPUT "${SRC_OUTPUT}")

            string(REPLACE " " ";" SRC_FLAGS ${CMAKE_${SRC_LANG}_FLAGS} " " ${CMAKE_${SRC_LANG}_FLAGS_${BUILD_TYPE}})

            add_custom_command(
                OUTPUT ${SRC_OUTPUT}
                DEPENDS ${SRC} ${SRC_PLAIN_OUTPUT}
                IMPLICIT_DEPENDS ${SRC_LANG} ${SRC}
                COMMAND ${SCOREP_EXECUTABLE} ${SCOREP_EXECUTABLE_FLAGS} ${CMAKE_${SRC_LANG}_COMPILER} ${SRC_FLAGS} ${TGT_DEFINITIONS} ${INC_DIRS} -c ${SRC} -o ${SRC_OUTPUT} > /dev/null 2>&1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Building Score-P ${SRC_LANG} object ${SRC_SHORT_OUTPUT}"
                VERBATIM
            )

            list(APPEND TGT_LINK_FILES ${SRC_OUTPUT})
        endforeach()

        string(REPLACE " " ";" TGT_FLAGS ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${tgt}_scorep
            COMMAND ${SCOREP_EXECUTABLE} ${SCOREP_EXECUTABLE_FLAGS} ${CMAKE_CXX_COMPILER} ${TGT_FLAGS} ${TGT_LINK_FILES} -o ${CMAKE_CURRENT_BINARY_DIR}/${tgt}_scorep ${TGT_LINK_LIBS} # > /dev/null 2>&1
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${TGT_LINK_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${tgt}
            COMMENT "Building Score-P instrumentated executable: ${tgt}_scorep"
            VERBATIM
        )

        add_custom_target(${tgt}_scorep ALL
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${tgt}_scorep)

        install(FILES $<TARGET_FILE:${tgt}>_scorep RUNTIME DESTINATION bin)
    endmacro()
ENDIF(NOT SCOREP_CONFIG)

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Scorep DEFAULT_MSG
    SCOREP_CONFIG
    SCOREP_LIBRARIES
    SCOREP_INCLUDE_DIRS
    SCOREP_EXECUTABLE)

if(SCOREP_COMPILER_INSTRUMENTATION)
    message(STATUS "    Using compiler instrumentation: YES")
else()
    message(STATUS "    Using compiler instrumentation: NO")
endif()

if(SCOREP_USER_INSTRUMENTATION)
    message(STATUS "    Using user     instrumentation: YES")
else()
    message(STATUS "    Using user     instrumentation: NO")
endif()
