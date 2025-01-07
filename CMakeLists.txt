cmake_minimum_required(VERSION 3.10)
project(croco VERSION 0.2.0)

set(CMAKE_C_STANDART 11)
set(CMAKE_C_STANDART_REQUIRED True)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set (SOURSES
	src/commands.c
	src/croco.c
	src/files.c
	src/util.c
)

add_executable(${PROJECT_NAME} ${SOURSES})

target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES})

