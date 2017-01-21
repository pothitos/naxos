cmake_minimum_required(VERSION 2.8)
add_compile_options(-pedantic -Wall -W -Wshadow -Werror -std=c++0x -O2
                    $ENV{COVERAGE})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s $ENV{COVERAGE}")

include_directories(../core/bounds-consistency)
add_subdirectory(../core/bounds-consistency ../core/bounds-consistency)

add_executable(nqueens nqueens.cpp)
target_link_libraries(nqueens naxos)

add_executable(send_more_money send_more_money.cpp)
target_link_libraries(send_more_money naxos)

enable_testing()
add_test(verification verification/test.sh)
