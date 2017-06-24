cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -pedantic -Wall -W -Wshadow -Werror -std=c++0x -O2 $ENV{COVERAGE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s -static-libstdc++ $ENV{COVERAGE}")

include_directories(../../core)
add_subdirectory(../../core ../../core)

include(ExternalProject)

ExternalProject_Add(XCSP3-Java-Tools
    PREFIX checker
    GIT_REPOSITORY https://github.com/xcsp3team/XCSP3-Java-Tools.git
    CONFIGURE_COMMAND ""
    BINARY_DIR checker/src/XCSP3-Java-Tools/src/main/java
    BUILD_COMMAND test -e org/xcsp/checker/SolutionChecker.class || \\
                  javac -classpath ../../../lib/javax.json-1.0.4.jar \\
                  -sourcepath . org/xcsp/checker/SolutionChecker.java
    INSTALL_COMMAND ""
)

ExternalProject_Add(XCSP3-CPP-Parser
    PREFIX parser
    GIT_REPOSITORY https://github.com/xcsp3team/XCSP3-CPP-Parser.git
    CONFIGURE_COMMAND ""
    BINARY_DIR parser/src/XCSP3-CPP-Parser/samples
    BUILD_COMMAND make lib
    INSTALL_COMMAND ""
)

add_library(parserxcsp3core STATIC IMPORTED)
set_target_properties(parserxcsp3core PROPERTIES IMPORTED_LOCATION
                      parser/src/XCSP3-CPP-Parser/lib/libparserxcsp3core.a)

include(FindLibXml2)
include_directories(SYSTEM ${LIBXML2_INCLUDE_DIR}
                    parser/src/XCSP3-CPP-Parser/include)

add_executable(naxos-xcsp3 naxos-xcsp3.cpp translator.cpp)
target_link_libraries(naxos-xcsp3 naxos-mini ${LIBXML2_LIBRARIES}
                      parserxcsp3core)
add_dependencies(naxos-xcsp3 XCSP3-CPP-Parser XCSP3-Java-Tools)

enable_testing()
add_test(verification verification/test.sh)
