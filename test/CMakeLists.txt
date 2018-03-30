cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# gtest need this
cmake_policy(SET CMP0057 NEW)

# include gtest
include(GoogleTest)

# project name
set(PROJECT_NAME UnitTest)

# main project
project(${PROJECT_NAME} CXX)

# definition of macros
add_definitions(
   -D_DEBUG 
   -DUNICODE
   -D_UNICODE
)

# include directories 
include_directories(../include)

# define flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lgtest -lpthread -luv")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# add source file
aux_source_directory(. SOURCES)

# add binary file
add_executable(${PROJECT_NAME} ${SOURCES})

# add gtest test case
gtest_add_tests(${PROJECT_NAME} "" AUTO)

