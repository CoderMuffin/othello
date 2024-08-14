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
if(WIN32)
    set(CompilerFlags /external:anglebrackets /external:W0 /W4 /WX /Wall /wd4514 /wd4668 /wd5045 /wd4820)
else()
    set(CompilerFlags -O3 -Wall -Wpedantic)
endif()

include_directories(lib)

add_library(othello "src/board.cpp" "src/nn.cpp" "src/util.cpp" "src/nnbatch.cpp" "src/input.cpp")

target_compile_options(othello PRIVATE ${CompilerFlags})

add_executable(main "src/main.cpp")
target_compile_options(main PRIVATE ${CompilerFlags})

add_executable(test "src/test.cpp")

target_link_libraries(main PRIVATE othello)
target_link_libraries(test PRIVATE othello)

