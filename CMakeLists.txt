cmake_minimum_required(VERSION 3.20)

project(Epp)

set(CMAKE_CXX_STANDARD 20)

file(GLOB HEADER_FILES "include/*.hpp")

add_executable(epp src/main.cpp ${HEADER_FILES})

include_directories(include)
