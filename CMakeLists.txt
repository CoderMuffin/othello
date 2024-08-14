cmake_minimum_required(VERSION 3.14)
if(${CMAKE_VERSION} VERSION_LESS 3.29)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.29)
endif()

project(othello)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(lib)

add_library(othello "src/board.cpp" "src/nn.cpp" "src/util.cpp" "src/nnbatch.cpp" "src/input.cpp")

target_compile_options(othello PRIVATE -O3)

add_executable(main "src/main.cpp" "src/input.cpp")
target_compile_options(main PRIVATE -O3 -g)

add_executable(test "src/test.cpp")

target_link_libraries(main PRIVATE othello)
target_link_libraries(test PRIVATE othello)

