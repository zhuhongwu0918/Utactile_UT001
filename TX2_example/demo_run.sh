#!/bin/bash  

can_mount=0
can_device=$1

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

    sudo ip link set $can_device type can bitrate 1000000 dbitrate 2000000 berr-reporting on fd on
    if [ $? -ne 0 ]; then
        echo "can 波特率设置失败"
        return
    fi

    sudo ip link set up $can_device &&
    if [ $? -ne 0 ]; then
        echo "can 启动失败"
        return
    fi

    can_mount=1
}

if [[ "$can_device" =~ can[0-9]+ ]];then
    echo "指定通信设备为 $can_device"
else
    echo "请输入正确的通信名称，如:can0"
    exit 0
fi

# 使用 ifconfig 检查指定通信设备设备是否存在  
can_device_list=$(ifconfig | grep -E '^can[0-9]+' | awk '{print $1}' | sed 's/://g')
for list in "${can_device_list[@]}"; do
    if [ "$list"x == "$can_device"x ];then
        can_mount=1
        echo "指定通信设备 $can_device 设备已挂载"
    fi
done

if [ $can_mount -eq 0 ]; then  
    echo "指定通信设备 $can_device 没有挂载,开始挂载该设备" 
    can_device_mount
    if [ $can_mount -eq 1 ]; then
        echo "指定通信设备 $can_device 已成功挂载" 
    else
        echo "指定通信设备 $can_device 挂载失败,请重试"
        exit 0
    fi
fi

ls | grep -E "build"
if [ $? -ne 0 ];then
    mkdir build
fi

cd build
cmake_output=$(cmake .. 2>&1)  
echo "$cmake_output"
if echo "$cmake_output" | grep -iq "error";then
    echo "cmake 执行出错，退出"
    exit 0
fi

make_output=$(make 2>&1)  
echo "$make_output"
if echo "$make_output" | grep -iq "error" ;then
    echo "代码编译出错"
    exit 0
fi
./main "$can_device"

exit 0
