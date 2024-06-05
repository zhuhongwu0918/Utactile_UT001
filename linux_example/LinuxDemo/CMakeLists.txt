cmake_minimum_required (VERSION 2.8)
 
project (skin_demo)

include_directories (${CMAKE_SOURCE_DIR}/Communication/Inc)

aux_source_directory (${CMAKE_SOURCE_DIR}/Communication/Src SRC_LIST1)
 
add_executable(skin_demo ${SRC_LIST1} main.cpp)

