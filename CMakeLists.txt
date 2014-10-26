cmake_minimum_required (VERSION 2.8)

project (htop)

add_definitions (-DNDEBUG -D_XOPEN_SOURCE_EXTENDED -DSYSCONFDIR="/usr/local/etc")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -rdynamic -std=c99 -O3")
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -Wall -Wextra -rdynamic std=c99 -g")

find_library (M_LIB m)
find_library (NCURSESW_LIB ncursesw)

file (GLOB sources *.c)

add_executable (htop ${sources})
target_link_libraries (htop ${M_LIB} ${NCURSESW_LIB})

install (TARGETS htop DESTINATION bin)
