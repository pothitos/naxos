cmake_minimum_required(VERSION 2.6)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 $ENV{COVERAGE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} $ENV{COVERAGE}")
else()
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -pedantic -Wall -W -Wshadow -Werror -std=c++0x -O2 $ENV{COVERAGE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s $ENV{COVERAGE}")
endif()

include_directories(../core)
add_subdirectory(../core ../core)

add_executable(nqueens nqueens.cpp)
target_link_libraries(nqueens naxos)

add_executable(send_more_money send_more_money.cpp)
target_link_libraries(send_more_money naxos)

enable_testing()
add_test(verification verification/test.sh)
