cmake_minimum_required(VERSION 2.6)
project(roco2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
        "MinSizeRel" "RelWithDebInfo")
endif()

include(CTest)

# Intialize git submodules if not done already
file(GLOB SUBMODULE_FILES "lib/nitro/*")
list(LENGTH SUBMODULE_FILES COUNT_NITRO)
if(${COUNT_NITRO} EQUAL 0)
    message(STATUS "Initializing git submodule")
    execute_process(COMMAND "git" "submodule" "init" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()
message(STATUS "Updating git submodule")
execute_process(COMMAND "git" "submodule" "update" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# enable C++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -pedantic")

#enable C99
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# einfach den code mit magic macros vollscheissen, bis es geht :o)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE")

# set additional flags for debug build
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

# set additional flags for release build
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3") # -funroll-loops")

find_package(OpenMP REQUIRED)
if (OpenMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

option(USE_MKL "Whether to use MKL or LAPACK for matmul kernel" ON)
if(USE_MKL)
    # set definition to use MKL in matmul kernel
    set(MKL_SDL "ON")
    find_package(MKL REQUIRED)
    if (MKL_FOUND)
        include_directories(SYSTEM ${MKL_INCLUDE_DIRS})

        add_definitions(-DUSE_MKL)
    endif()
else()
    find_package(LAPACK REQUIRED)
    add_definitions(-DUSE_CBLAS)
endif()

# quirk for CMAKE 3.0 and newer
cmake_policy(SET CMP0026 OLD)

find_package(Scorep REQUIRED)
if (SCOREP_FOUND)
    include_directories(SYSTEM ${SCOREP_INCLUDE_DIRS})
endif()

find_package(LibNuma REQUIRED)
if (LIBNUMA_FOUND)
    include_directories(SYSTEM ${LIBNUMA_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "libnuma wasn't found, but is mandatory.")
endif()

find_package(CpuFreq REQUIRED)
if (CPUFREQ_FOUND)
    include_directories(SYSTEM ${CPUFREQ_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "cpufreq wasn't found, but is mandatory.")
endif()

find_package(X86Adapt)
if (X86Adapt_FOUND)
    add_definitions(-DHAS_X86ADAPT)
    include_directories(SYSTEM ${X86ADAPT_INCLUDE_DIRS})
else()
    message(STATUS "X86Adapt wasn't found, DDCM not available.")
endif()

include_directories(include)

option(ROCO2_ASSERTIONS "enable additional checks with runtime overhead." OFF)

if(ROCO2_ASSERTIONS)
    message(STATUS "Additional runtime checks enabled. This may influence the measurement")
    add_definitions(-DROCO2_ASSERTIONS)
endif()

add_definitions(-DREGONLY)

include(lib/nitro/Nitro.cmake)

include(cmake/Roco2Configuration.cmake)

add_subdirectory(src)
