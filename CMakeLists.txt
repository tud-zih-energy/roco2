cmake_minimum_required(VERSION 3.11)
project(roco2)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# Set a default build type if none was specified
include(cmake/DefaultBuildType.cmake)

include(CTest)

# Intialize git submodules if not done already
include(cmake/GitSubmoduleUpdate.cmake)
git_submodule_update()

add_library(roco2 INTERFACE)

target_compile_features(roco2 INTERFACE cxx_std_14)
target_compile_options(roco2 INTERFACE $<$<CONFIG:Debug>:-O0> -Wall -Wextra)

#enable C99
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

# einfach den code mit magic macros vollscheissen, bis es geht :o)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_POSIX_C_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_GNU_SOURCE")

find_package(OpenMP REQUIRED)
target_link_libraries(roco2 INTERFACE OpenMP::OpenMP_CXX)

# this should also find MKL
find_package(LAPACK REQUIRED)
target_link_libraries(roco2 INTERFACE ${LAPACK_LIBRARIES} ${LAPACK_LINKER_FLAGS})

find_package(Scorep)
option(USE_SCOREP "Enable built-in Score-P user instrumentation during build." OFF)
if(Scorep_FOUND AND USE_SCOREP)
    target_link_libraries(roco2 INTERFACE Scorep::Plugin)
    target_compile_definitions(roco2 INTERFACE HAS_SCOREP)
    message(STATUS "Building WITH Score-P user instrumentation. Make sure you have set up the Score-P compiler wrapper as Compiler!")
else()
    message(STATUS "Building WITHOUT Score-P user instrumentation.")
endif()

find_package(NUMA REQUIRED)
target_link_libraries(roco2 INTERFACE NUMA::NUMA)

find_package(CpuFreq REQUIRED)
target_link_libraries(roco2 INTERFACE Cpufreq::Cpufreq)

find_package(X86Adapt)
if (X86Adapt_FOUND)
    target_compile_definitions(roco2 INTERFACE HAS_X86ADAPT)
    target_link_libraries(roco2 INTERFACE X86Adapt::X86Adapt)
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
target_link_libraries(roco2 INTERFACE Nitro::log)

macro(Roco2Configuration CONF )
    add_executable(roco2_${CONF} experiment.cpp)

    target_link_libraries(
        roco2_${CONF}
        # fastcpufreq
        # ${FIRESTARTER_LIBRARIES}
        roco2_core
        roco2_main
    )

    install(TARGETS roco2_${CONF} RUNTIME DESTINATION bin)
endmacro()

add_subdirectory(src)
