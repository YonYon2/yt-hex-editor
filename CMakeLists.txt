cmake_minimum_required(VERSION 3.31)
project(my_hex_edit)

set(CMAKE_CXX_STANDARD 26)

add_executable(${PROJECT_NAME} main.cpp)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)