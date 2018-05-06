cmake_minimum_required (VERSION 2.8)

add_subdirectory(net)
add_subdirectory(console)
add_subdirectory(crypto)

include_directories(console)

set(SRC
	client.c)

set(LIBS
	console
	crypto)

add_executable(client ${SRC})

target_link_libraries(client ${LIBS})
