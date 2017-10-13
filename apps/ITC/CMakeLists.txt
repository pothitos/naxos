cmake_minimum_required(VERSION 2.6)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 $ENV{COVERAGE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} $ENV{COVERAGE}")
else()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -pedantic -Wall -W -Wshadow -Werror -std=c++0x -O2 $ENV{COVERAGE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s $ENV{COVERAGE}")
endif()

include_directories(../../core)
add_subdirectory(../../core ../../core)

include_directories(../../plugins)
add_subdirectory(../../plugins ../../plugins)

add_executable(itc_solver heuristics.cpp io.cpp localSearch.cpp timetabling.cpp)
target_link_libraries(itc_solver naxos amorgos)

enable_testing()
add_test(verification verification/test.sh)
