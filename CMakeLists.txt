cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# project name
set(PROJECT_NAME uvx)

# main project
project(${PROJECT_NAME} CXX)

# enable testing
enable_testing()

# add unit test
add_subdirectory(test)

