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

add_executable(nqueens nqueens.cpp)
target_link_libraries(nqueens naxos)

add_executable(numpart numpart.cpp)
target_link_libraries(numpart naxos)
