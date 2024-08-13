cmake_minimum_required (VERSION 2.8)
 
project (skin_demo)

find_package(Threads REQUIRED)
 
add_executable(main decode_can.cpp can_config.cpp main.cpp)

target_link_libraries(main PUBLIC Threads::Threads)

