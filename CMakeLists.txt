cmake_minimum_required(VERSION 3.23)
project(croco VERSION 0.2.0)

find_package(Curses REQUIRED)

# FindCurses hasn't been updated to use modern CMake, so do it ourselves
add_library(Curses::Curses INTERFACE IMPORTED)
target_include_directories(Curses::Curses INTERFACE ${CURSES_INCLUDE_DIRS})
target_link_libraries(Curses::Curses INTERFACE ${CURSES_LIBRARIES})
target_compile_options(Curses::Curses INTERFACE ${CURSES_CFLAGS})

add_executable(croco)

target_compile_features(croco
  PRIVATE
    c_std_11
)

target_sources(croco
  PRIVATE
    src/commands.c
    src/croco.c
    src/files.c
    src/util.c

  PRIVATE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(croco 
  PRIVATE 
    Curses::Curses
)
