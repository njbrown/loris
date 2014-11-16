cmake_minimum_required(VERSION 2.6)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(dragonscript)

file(GLOB_RECURSE DS_SOURCES "src/*.cpp")
add_library(dragonscript ${DS_SOURCES})