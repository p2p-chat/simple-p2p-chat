cmake_minimum_required (VERSION 3.5)

add_definitions(-Wall)

# add_library(net net.c)
add_executable(client client.c net.c ../view/view.c)
add_executable(server server.c net.c ../view/view.c)