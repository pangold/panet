cmake_minimum_required (VERSION 2.8)

set (name pango CACHE PATH "name of project")
project (${name})

set (dir_inc_boost "/usr/local/include" CACHE PATH "include of boost")
set (dir_lib_boost "/usr/local/lib" CACHE PATH "lib of boost")
set (dir_inc_pan "." CACHE PATH "include of pango")
set (dir_inc_protobuf "" CACHE PATH "include of protobuf")
set (dir_lib_protobuf "" CACHE PATH "lib of protobuf")

set (dir_inc ${dir_inc_boost} ${dir_inc_pan} ${dir_inc_protobuf})
set (dir_lib ${dir_lib_boost} ${dir_lib_protobuf})

set (as_server FALSE CACHE BOOL "use as server")

if (as_server) 
add_definitions (-DAS_SERVER)
elseif ()
add_definetions (-DAS_CLIENT)
endif ()

if (WIN32)
add_definitions (-D_CRT_SECURE_NO_WARNINGS)
elseif ()
add_definitions (-std=c++14)
endif ()

include_directories (${dir_inc})
link_directories (${dir_lib})

set (link_protobuf 
    debug libprotobufd.lib optimized libprotobuf.lib
    debug libprotobuf-lited.lib optimized libprotobuf-lite.lib
    debug libprotocd.lib optimized libprotoc.lib
)

add_subdirectory (test)

add_executable (${name} hello.cpp)

target_link_libraries (${name} ${link_protobuf})
