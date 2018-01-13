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

add_executable(cryptmult cryptmult.cpp)
target_link_libraries(cryptmult naxos amorgos)

add_executable(golfer golfer.cpp)
target_link_libraries(golfer naxos amorgos)

add_executable(graphcolor graphcolor.cpp)
target_link_libraries(graphcolor naxos amorgos)

add_executable(knump knump.cpp)
target_link_libraries(knump naxos amorgos)

add_executable(magic magic.cpp)
target_link_libraries(magic naxos amorgos)

add_executable(mapcolor mapcolor.cpp)
target_link_libraries(mapcolor naxos amorgos)

add_executable(nqueens nqueens.cpp)
target_link_libraries(nqueens naxos amorgos)

add_executable(placement placement.cpp)
target_link_libraries(placement naxos amorgos)

add_executable(sudoku sudoku.cpp)
target_link_libraries(sudoku naxos amorgos)

add_executable(vertexcover vertexcover.cpp)
target_link_libraries(vertexcover naxos amorgos)
