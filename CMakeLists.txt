cmake_minimum_required(VERSION 3.16)

include(cmake/vcpkg.cmake)

project(hello LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
