cmake_minimum_required (VERSION 2.8)
 
project (skin_demo)

find_package(Threads REQUIRED)

include_directories (${CMAKE_SOURCE_DIR}/Communication/Inc)

aux_source_directory (${CMAKE_SOURCE_DIR}/Communication/Src SRC_LIST)
 
add_executable(skin_demo ${SRC_LIST} main.cpp)

target_link_libraries(skin_demo PUBLIC Threads::Threads)

