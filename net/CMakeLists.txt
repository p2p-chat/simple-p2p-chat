cmake_minimum_required (VERSION 3.5)

project(net)

add_definitions(-Wall)

add_library(net SHARED net.c)