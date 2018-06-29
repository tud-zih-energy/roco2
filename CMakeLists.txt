cmake_minimum_required(VERSION 2.6)
project(roco2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# Set a default build type if none was specified
include(cmake/DefaultbuildType.cmake)

include(CTest)

# Intialize git submodules if not done already
include(cmake/GitSubmoduleUpdate.cmake)
git_submodule_update()

add_library(roco2 INTERFACE)

# enable C++11
target_compile_features(roco2 INTERFACE cxx_std_14)
target_compile_options(roco2 INTERFACE $<$<CONFIG:Debug>:-O0 -Werror> -Wall -Wextra -pedantic)

#enable C99
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# einfach den code mit magic macros vollscheissen, bis es geht :o)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE")


find_package(OpenMP REQUIRED)
target_link_libraries(roco2 INTERFACE OpenMP::OpenMP_CXX)

# option(USE_MKL "Whether to use MKL or LAPACK for matmul kernel" ON)
# if(USE_MKL)
#     # set definition to use MKL in matmul kernel
#     set(MKL_SDL "ON")
#     find_package(MKL REQUIRED)
#     if (MKL_FOUND)
#         include_directories(SYSTEM ${MKL_INCLUDE_DIRS})
#
#         add_definitions(-DUSE_MKL)
#     endif()
# else()
#     find_package(LAPACK REQUIRED)
#     add_definitions(-DUSE_CBLAS)
# endif()

# this should also find MKL
find_package(LAPACK REQUIRED)
target_link_libraries(roco2 INTERFACE ${LAPACK_LIBRARIES} ${LAPACK_LINKER_FLAGS})
target_compile_definitions(roco2 INTERFACE USE_CBLAS)


# quirk for CMAKE 3.0 and newer
# cmake_policy(SET CMP0026 OLD)

# find_package(Scorep REQUIRED)
# if (SCOREP_FOUND)
#     include_directories(SYSTEM ${SCOREP_INCLUDE_DIRS})
# endif()

find_package(LibNuma REQUIRED)
if (LIBNUMA_FOUND)
    target_include_directories(roco2 INTERFACE SYSTEM ${LIBNUMA_INCLUDE_DIRS})
    target_link_libraries(roco2 INTERFACE ${LIBNUMA_LIBRARIES})
else()
    message(FATAL_ERROR "libnuma wasn't found, but is mandatory.")
endif()

find_package(CpuFreq REQUIRED)
if (CPUFREQ_FOUND)
    target_include_directories(roco2 INTERFACE SYSTEM ${CPUFREQ_INCLUDE_DIRS})
    target_link_libraries(roco2 INTERFACE ${CPUFREQ_LIBRARIES})
else()
    message(FATAL_ERROR "cpufreq wasn't found, but is mandatory.")
endif()

find_package(X86Adapt)
if (X86Adapt_FOUND)
    target_compile_definitions(roco2 INTERFACE HAS_X86ADAPT)
    target_include_directories(roco2 INTERFACE SYSTEM ${X86ADAPT_INCLUDE_DIRS})
    target_link_libraries(roco2 INTERFACE ${X86ADAPT_LIBRARIES})
else()
    message(STATUS "X86Adapt wasn't found, DDCM not available.")
endif()

include(cmake/BuildFirestarter.cmake)

target_include_directories(roco2 INTERFACE include)

option(ROCO2_ASSERTIONS "enable additional checks with runtime overhead." OFF)

if(ROCO2_ASSERTIONS)
    message(STATUS "Additional runtime checks enabled. This may influence the measurement")
    target_compile_definitions(roco2 INTERFACE ROCO2_ASSERTIONS)
endif()

target_compile_definitions(roco2 INTERFACE REGONLY)

add_subdirectory(lib/nitro)
target_link_libraries(roco2 INTERFACE Nitro::nitro-log)

macro(Roco2Configuration CONF )
    add_executable(roco2_${CONF} experiment.cpp)

    target_link_libraries(
        roco2_${CONF}
        # fastcpufreq
        # ${FIRESTARTER_LIBRARIES}
        roco2_core
        roco2_main
    )

    # add_scorep_instrumentation(roco2_${CONF})

    install(TARGETS roco2_${CONF} RUNTIME DESTINATION bin)
endmacro()

add_subdirectory(src)
