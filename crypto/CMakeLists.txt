cmake_minimum_required (VERSION 2.8)

add_definitions(-Wall)

add_library(rsa rsa.c)

target_link_libraries(rsa m)

#[[test]]
add_executable(test_crypto test.c)
target_link_libraries(test_crypto rsa)