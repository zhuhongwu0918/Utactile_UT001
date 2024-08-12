cmake_minimum_required(VERSION 3.16)
project(skin_demo)

set(dirRoot "${CMAKE_SOURCE_DIR}")
set(dirShareLib "${dirRoot}/Libs")  
set(fileShareLib "${dirShareLib}/ControlCAN${CMAKE_SHARED_LIBRARY_SUFFIX}")
set(dirTarget "${dirRoot}/build")

include_directories(${CMAKE_SOURCE_DIR}/Libs)

find_library(ControlCAN NAMES Libs/ControlCAN.lib)
# 检查是否找到了ControlCAN库  
if(ControlCAN)  
    message(STATUS "Found ControlCAN.lib: ${ControlCAN}")  
else()  
    message(FATAL_ERROR "ControlCAN.lib library not found")  
endif()  

add_executable(main main.cpp usb_can.cpp decode_can.cpp)

target_link_libraries(main ${ControlCAN})
  
# 使用 add_custom_command 和 add_custom_target 来拷贝动态库  
# 注意：这里使用了 CMake 的 file(COPY ...) 命令，它在 CMake 3.13 及更高版本中可用  
# 如果你使用的是旧版本的 CMake，你可能需要使用 execute_process() 调用 cp 或 copy 命令  
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)  
    add_custom_command(TARGET main POST_BUILD  
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${fileShareLib}" "${dirTarget}"  
        COMMENT "Copying dynamic library to executable output directory"  
    )  
else()  
    # 对于旧版本的 CMake，使用 execute_process 调用系统命令  
    # 注意：这里以 Windows 为例，Linux 需要使用 cp 命令  
    add_custom_command(TARGET main POST_BUILD  
        COMMAND ${CMAKE_COMMAND} -E execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${fileShareLib}" "${dirTarget}" RESULT_VARIABLE COPY_RESULT)  
        COMMAND if NOT ${COPY_RESULT} EQUAL 0 (message(FATAL_ERROR "Failed to copy dynamic library to executable output directory"))  
        COMMENT "Copying dynamic library to executable output directory (using old CMake version)"  
    )  
endif() 


