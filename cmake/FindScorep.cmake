##
## This file is part of haec_sim (https://tu-dresden.de/zih/haec_sim)
## haec_sim - A trace-based parallel discrete event simulation framework
##
## Copyright (c) 2013-2017 Technische Universitaet Dresden, Germany
##
## haec_sim is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## haec_sim is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with haec_sim.  If not, see <http://www.gnu.org/licenses/>.
##

if (SCOREP_LIBRARIES AND SCOREP_INCLUDE_DIRS)
  set (Scorep_FIND_QUIETLY TRUE)
endif ()

# Yada Yada, LOCATION property deprecated something something
# Whatever, shut up CMake, I don't know what I'm doing, but you don't either.
cmake_policy(SET CMP0026 OLD)

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

    option(SCOREP_INSTRUMENTATION_COMPILER "Enable Score-P compiler instrumentation" OFF)
    option(SCOREP_INSTRUMENTATION_USER "Enable Score-P user instrumentation" ON)
    option(SCOREP_ONLINE_ACCESS "Enables Score-P online acces to be used with PTF" ON)
    option(SCOREP_STATIC "Enable Score-P static instrumentation (otherwise use shared)" ON)
    option(SCOREP_INSTRUMENTATION_MPI "Forcibly enables MPI instrumentation" OFF)

    set(SCOREP_INSTRUMENTATION_FILTER "${CMAKE_SOURCE_DIR}/share/scorep/compile.flt" CACHE
        FILEPATH "Path to a filter file used for compile-time filtering of Score-P")

    if(SCOREP_INSTRUMENTATION_MPI)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--mpp=mpi")
    endif()

    if(SCOREP_STATIC)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--static")
    else()
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--shared")

    endif()

    if(SCOREP_INSTRUMENTATION_COMPILER)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--compiler")
    else()
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--nocompiler")
    endif()

    if(SCOREP_INSTRUMENTATION_USER)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--user")
    else()
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--nouser")
    endif()

    if(SCOREP_ONLINE_ACCESS)
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--online-access")
    else()
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--noonline-access")
    endif()
            

    if(EXISTS ${SCOREP_INSTRUMENTATION_FILTER})
        set(SRC_FILTER_DEP ${SCOREP_INSTRUMENTATION_FILTER})
        list(APPEND SCOREP_EXECUTABLE_FLAGS "--instrument-filter=${SCOREP_INSTRUMENTATION_FILTER}")
    else()
        set(SRC_FILTER_DEP)
    endif()

    macro(add_scorep_instrumentation tgt)

        get_target_property(TGT_INC ${tgt} "INCLUDE_DIRECTORIES")
        set(INC_DIRS)
        foreach(INC_DIR ${TGT_INC})
            string(STRIP "${INC_DIR}" INC_DIR)
            list(APPEND INC_DIRS "-I${INC_DIR}")
        endforeach()

        get_target_property(TGT_LIBS ${tgt} "LINK_LIBRARIES")
        set(TGT_LINK_LIBS)
        set(TGT_LINK_RPATH)
        set(TGT_LINK_DEPS)
        if(TGT_LIBS)
            foreach(LIB_TGT ${TGT_LIBS})
                if(TARGET ${LIB_TGT})
                    get_property(LIB_PATH TARGET "${LIB_TGT}" PROPERTY LOCATION)
                else()
                    string(STRIP "${LIB_TGT}" LIB_TGT)
                    set(LIB_PATH ${LIB_TGT})
                endif()

                get_filename_component(LIB_DIR ${LIB_PATH} DIRECTORY)
                get_filename_component(LIB_NAME ${LIB_PATH} NAME_WE)
                get_filename_component(LIB_EXT ${LIB_PATH} EXT)
                string(REPLACE "lib" "" LIB_SHORT_NAME ${LIB_NAME})

message("${LIB_PATH}")

                    list(APPEND TGT_LINK_RPATH "-Wl,-rpath,/sw/global/compilers/gcc/7.1.0/lib64")
                if("${LIB_EXT}" STREQUAL ".so")
                    list(APPEND TGT_LINK_RPATH "-Wl,-rpath,${LIB_DIR}")
                endif()

                if(TARGET "${LIB_SHORT_NAME}_scorep")
                    SET(LIB_INSTRUMENTED "${LIB_DIR}/${LIB_NAME}_scorep${LIB_EXT}")
                    list(APPEND TGT_LINK_DEPS "${LIB_INSTRUMENTED}")
                    list(APPEND TGT_LINK_LIBS "${LIB_INSTRUMENTED}")
                else()
                    if(TARGET ${LIB_SHORT_NAME})
                        list(APPEND TGT_LINK_DEPS "${LIB_PATH}")
                    endif()
                    list(APPEND TGT_LINK_LIBS "${LIB_PATH}")
                endif()
            endforeach()
        endif()

        get_directory_property(TGT_DEFS "COMPILE_DEFINITIONS")
        set(TGT_DEFINITIONS)
        foreach(TGT_DEF ${TGT_DEFS})
            string(STRIP "${TGT_DEF}" TGT_DEF)
            list(APPEND TGT_DEFINITIONS "-D${TGT_DEF}")
        endforeach()

        string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)

        get_target_property(TGT_SRCS ${tgt} "SOURCES")
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

            set(SRC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${tgt}_scorep.dir/${SRC_DIR}")

            file(MAKE_DIRECTORY ${SRC_OUTPUT_DIR})

            set(SRC_OUTPUT "${SRC_OUTPUT_DIR}/${SRC_NAME}.${SRC_LANG_LOWER}.o")
            set(SRC_PLAIN_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${tgt}.dir/${SRC_DIR}/${SRC_NAME}.${SRC_LANG_LOWER}.o")

            string(REPLACE "${CMAKE_BINARY_DIR}/" "" SRC_SHORT_OUTPUT "${SRC_OUTPUT}")

            string(REPLACE " " ";" SRC_FLAGS ${CMAKE_${SRC_LANG}_FLAGS} " " ${CMAKE_${SRC_LANG}_FLAGS_${BUILD_TYPE}})

            get_property(tgt_fpic TARGET ${tgt} PROPERTY POSITION_INDEPENDENT_CODE)
            if(tgt_fpic)
                list(APPEND SRC_FLAGS "-fPIC")
            endif()

            # remove -pedantic, as it causes "style of line directive is a GCC extension" warnings
            list(REMOVE_ITEM SRC_FLAGS "-pedantic")

            add_custom_command(
                OUTPUT ${SRC_OUTPUT}
                DEPENDS ${SRC} ${SRC_PLAIN_OUTPUT} ${SRC_FILTER_DEP}
                IMPLICIT_DEPENDS ${SRC_LANG} ${SRC}
                COMMAND ${SCOREP_EXECUTABLE} ${SCOREP_EXECUTABLE_FLAGS} ${CMAKE_${SRC_LANG}_COMPILER} ${SRC_FLAGS} ${TGT_DEFINITIONS} ${INC_DIRS} -c ${SRC} -o ${SRC_OUTPUT} #> /dev/null 2>&1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Building Score-P ${SRC_LANG} object ${SRC_SHORT_OUTPUT}"
                VERBATIM
            )

            list(APPEND TGT_LINK_FILES ${SRC_OUTPUT})
        endforeach()

        string(REPLACE " " ";" TGT_FLAGS ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

        get_property(tgt_type TARGET ${tgt} PROPERTY TYPE)

        get_property(tgt_location TARGET ${tgt} PROPERTY LOCATION)
        get_filename_component(tgt_file_dir ${tgt_location} DIRECTORY)
        get_filename_component(tgt_file_name ${tgt_location} NAME_WE)
        get_filename_component(tgt_file_ext ${tgt_location} EXT)

        set(tgt_output "${tgt_file_dir}/${tgt_file_name}_scorep${tgt_file_ext}")

        if(${tgt_type} STREQUAL "EXECUTABLE")
            message(STATUS "Add Score-P instrumentation for executable ${tgt}")
message("${TGT_FLAGS} £$% ${TGT_LINK_FILES} £$% ${TGT_LINK_RPATH}")
            add_custom_command(
                OUTPUT ${tgt_output}
                COMMAND ${SCOREP_EXECUTABLE} ${SCOREP_EXECUTABLE_FLAGS} ${CMAKE_CXX_COMPILER} ${TGT_FLAGS} ${TGT_LINK_FILES} -o ${tgt_output} ${TGT_LINK_RPATH} ${TGT_LINK_LIBS} # > /dev/null 2>&1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                DEPENDS ${TGT_LINK_FILES} ${tgt} ${TGT_LINK_DEPS}
                COMMENT "Linking Score-P instrumentated executable: ${tgt}_scorep"
                VERBATIM
            )

            install(FILES ${tgt_output} DESTINATION bin)
        elseif(${tgt_type} STREQUAL "STATIC_LIBRARY")
            message(STATUS "Add Score-P instrumentation for static library ${tgt}")
            add_custom_command(
                OUTPUT ${tgt_output}
                COMMAND ${CMAKE_AR} -rcs ${tgt_output} ${TGT_LINK_FILES} ${TGT_LINK_LIBS} # > /dev/null 2>&1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                DEPENDS ${TGT_LINK_FILES} ${tgt} ${TGT_LINK_DEPS}
                COMMENT "Linking Score-P instrumentated static library: ${tgt}_scorep"
                VERBATIM
            )
            install(FILES ${tgt_output} DESTINATION lib)
        elseif(${tgt_type} STREQUAL "SHARED_LIBRARY")
            message(STATUS "Add Score-P instrumentation for shared library ${tgt}")

            if(${tgt_file_ext} STREQUAL ".dylib")
                set(LINKER_LIB_FLAG "-dynamiclib")
            else()
                set(LINKER_LIB_FLAG "-shared")
            endif()

            add_custom_command(
                OUTPUT ${tgt_output}
                COMMAND ${SCOREP_EXECUTABLE} ${SCOREP_EXECUTABLE_FLAGS} ${CMAKE_CXX_COMPILER} ${LINKER_LIB_FLAG} ${TGT_LINK_FILES} -o ${tgt_output} ${TGT_LINK_LIBS} # > /dev/null 2>&1
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                DEPENDS ${TGT_LINK_FILES} ${tgt} ${TGT_LINK_DEPS}
                COMMENT "Linking Score-P instrumentated shared library: ${tgt}_scorep"
                VERBATIM
            )
            install(FILES ${tgt_output} DESTINATION lib)
        else()
            message(SEND_ERROR "The target ${tgt} has the unsupported type ${tgt_type}")
        endif()

        add_custom_target(${tgt}_scorep ALL DEPENDS ${tgt_output})
    endmacro()
ENDIF(NOT SCOREP_CONFIG)

include (FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Scorep DEFAULT_MSG
    SCOREP_CONFIG
    SCOREP_LIBRARIES
    SCOREP_INCLUDE_DIRS
    SCOREP_EXECUTABLE)

if(SCOREP_INSTRUMENTATION_COMPILER)
    message(STATUS "    Using compiler instrumentation: YES")
else()
    message(STATUS "    Using compiler instrumentation: NO")
endif()

if(SCOREP_INSTRUMENTATION_USER)
    message(STATUS "    Using user     instrumentation: YES")
else()
    message(STATUS "    Using user     instrumentation: NO")
endif()

if(SCOREP_ONLINE_ACCESS)
    message(STATUS "    Using online   access: YES")
else()
    message(STATUS "    Using online   access: NO")
endif()

