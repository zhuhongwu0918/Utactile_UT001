#!/bin/bash  

can_mount=0

function can_device_mount()
{
    sudo modprobe can
    if [ $? -ne 0 ]; then
        echo "加载 can 模块失败"
        return
    fi

    sudo modprobe can_raw
    if [ $? -ne 0 ]; then
        echo "加载 can_raw 模块失败"
        return
    fi

    sudo modprobe can_bcm
    if [ $? -ne 0 ]; then
        echo "加载 can_bcm 模块失败"
        return
    fi

    sudo modprobe can_gw
    if [ $? -ne 0 ]; then
        echo "加载 can_gw 模块失败"
        return
    fi

    sudo modprobe can_dev
    if [ $? -ne 0 ]; then
        echo "加载 can_dev 模块失败"
        return
    fi

    sudo modprobe mttcan
    if [ $? -ne 0 ]; then
        echo "加载 mttcan 模块失败"
        return
    fi

    sudo ip link set can0 type can bitrate 1000000 dbitrate 2000000 berr-reporting on fd on
    if [ $? -ne 0 ]; then
        echo "can 波特率设置失败"
        return
    fi

    sudo ip link set up can0 &&
    if [ $? -ne 0 ]; then
        echo "can 启动失败"
        return
    fi

    can_mount=1
}
  
# 使用 ifconfig 检查 can0 设备是否存在  
can_device=$(ifconfig | grep -E '^can[0-9]+:' | awk -F':' '{print $1}')
if [ "$can_device"x == ""x ]; then  
    echo "没有挂载任何CAN设备,开始挂载can设备" 
    can_device_mount
    if [ $can_mount -eq 1 ]; then
        can_device=$(ifconfig | grep -E '^can[0-9]+:' | awk -F':' '{print $1}')
        echo "$can_device 已成功挂载" 
    fi
else  
    echo "$can_device 已挂载"  
fi

ls | grep -E "build"
if [ $? -ne 0 ];then
    mkdir build
fi

    cd build
    cmake ..
    make
    ./main

exit 0
