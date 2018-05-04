cmake_minimum_required (VERSION 3.5)

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
