cmake_minimum_required (VERSION 3.0)

set (name example)
project (${name})

# set where protobuf message files located.
set (pub_sub_pb_file ../include/pan/net/pub_sub/pubsub.pb.cc)
set (rpc_pb_file ../include/pan/net/rpc/rpc.pb.cc)
set (pattern_pb_file ../include/pan/net/pattern/pattern.pb.cc)

# set where dependent libraries located.
set (dir_inc_pango ../include CACHE PATH "include of pango")
set (dir_inc_protobuf E:/protobuf/src CACHE PATH "include of protobuf")
set (dir_lib_protobuf E:/protobuf/lib CACHE PATH "lib of protobuf")
set (dir_inc_boost E:/boost_1_68_0 CACHE PATH "include of boost")
set (dir_lib_boost E:/boost_1_68_0/stage/lib CACHE PATH "lib of boost")
set (dir_inc ${dir_inc_pango} ${dir_inc_protobuf} ${dir_inc_boost})
set (dir_lib ${dir_lib_protobuf} ${dir_lib_boost})

# set prefix "lib*"
if (WIN32)
  set (prefix lib)
endif (WIN32)

# set link of protobuf
set (link_lib_protobuf
  debug ${prefix}protobufd optimized ${prefix}protobuf
  debug ${prefix}protobuf-lited optimized ${prefix}protobuf-lite
  debug ${prefix}protocd optimized ${prefix}protoc
  )
set (link_lib ${link_lib_protobuf})

# 
if (WIN32)
  set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
else (WIN32)
  add_definitions (-std=c++14)
endif (WIN32)

# 
include_directories (${dir_inc})
link_directories (${dir_lib})

# 

# get all *.cpp in current folder
aux_source_directory(. src_file)

# make a project for each examples
foreach (src ${src_file})
  message ("file ${src} found")

  # project name
  string (REGEX REPLACE ".*/(.*)/*.cpp$" "\\1" pro_name ${src})

  # replace "_" to "-"
  string (REPLACE "_" "-" pro_name ${pro_name})
  message ("project ${pro_name} added")

  # src files(*.cpp files)
  set (file ${src})

  # add pubsub.pb.cc(protobuf message src file) to "pub/sub" project
  if (${pro_name} MATCHES "pub*" OR ${pro_name} MATCHES "sub*")
    message ("${pro_name} is pub/sub project")
    set (file ${file} ${pub_sub_pb_file})
  endif ()
  
  # add rpc.pb.cc(protobuf message src file) to "rpc" project
  if (${pro_name} MATCHES "rpc*")
    message ("${pro_name} is rpc project")
    set (file ${file} ${rpc_pb_file})
  endif ()
  
  # add pattern.pb.cc(protobuf message src file) to "pattern" project
  if (${pro_name} MATCHES "pattern*")
    message ("${pro_name} is pattern project")
    set (file ${file} ${pattern_pb_file})
  endif ()

  add_executable(${pro_name} ${file})
  target_link_libraries (${pro_name} ${link_lib})
endforeach (src)