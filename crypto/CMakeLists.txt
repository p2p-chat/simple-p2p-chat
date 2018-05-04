cmake_minimum_required (VERSION 3.5)

add_definitions(-Wall)

add_library(crypto rsa.c)

target_link_libraries(crypto m)

#[[test]]
add_executable(test_crypto test.c)
target_link_libraries(test_crypto crypto)