cmake_minimum_required(VERSION 2.6)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -W -Wshadow -Werror -std=c++0x -O2 $ENV{COVERAGE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s $ENV{COVERAGE}")

include_directories(../../core/bounds-consistency)
add_subdirectory(../../core/bounds-consistency ../../core/bounds-consistency)

add_executable(naxos-xcsp3 naxos-xcsp3.cpp)
target_link_libraries(naxos-xcsp3 naxos)

enable_testing()
add_test(verification verification.sh)
