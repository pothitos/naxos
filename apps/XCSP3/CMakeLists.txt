cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -W -Wshadow -Werror -std=c++0x -O2 $ENV{COVERAGE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s $ENV{COVERAGE}")

include_directories(../../core/bounds-consistency)
add_subdirectory(../../core/bounds-consistency ../../core/bounds-consistency)

include(ExternalProject)
ExternalProject_Add(XCSP3-CPP-Parser
    PREFIX parser
    GIT_REPOSITORY https://github.com/pothitos/XCSP3-CPP-Parser.git
    CONFIGURE_COMMAND ""
    BINARY_DIR parser/src/XCSP3-CPP-Parser/samples
    BUILD_COMMAND make lib
    INSTALL_COMMAND ""
)

add_library(parserxcsp3core STATIC IMPORTED)
set_target_properties(parserxcsp3core PROPERTIES IMPORTED_LOCATION
                      parser/src/XCSP3-CPP-Parser/lib/libparserxcsp3core.a)

add_executable(naxos-xcsp3 naxos-xcsp3.cpp)
target_link_libraries(naxos-xcsp3 naxos parserxcsp3core)
add_dependencies(naxos-xcsp3 parserxcsp3core)

enable_testing()
add_test(verification verification.sh)
