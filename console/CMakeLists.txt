cmake_minimum_required (VERSION 3.5)

add_definitions(-Wall)

add_library(console console.c)

target_link_libraries(console net)