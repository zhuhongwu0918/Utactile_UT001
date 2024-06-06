# 用户指南User Guide
# Linux平台下使用触觉传感器(RS485)
## 1. 环境要求
本例程中使用Cmake组织工程，在Ubuntu 20.04 LTS下编译通过。
## 3. 连接设备
将RS485转USB转换器连接至电脑；
查看设备对应的端口号，

`ls /dev/ttyUSB*`或
`ls /dev/ttyACM*`

如确认端口为ttyUSB0，给端口赋予读写权限，如

`sudo chmod 777 /dev/ttyUSB0`


## 2. 软件包的使用
打开main.cpp文件，找到
`demo_app_init();`并跳转至app.cpp文件；

配置待读取的端口号，如
`char device[] = "/dev/ttyUSB0";`
配置读取的波特率，为115200，如
`cfg.baudrate = 115200;`

进入linux_example/LinuxDemo/目录下，执行如下命令：

`mkdir build`

`cd build`

`cmake ..`

`make`

生成skin_demo可执行文件并执行
`./skin_demo`
得到如下图所示的数据输出：

![linux_example_output](pictures/linux_example_output.png)


数据都存储`app.cpp`文件中的函数`void axit_decode(unsigned char *pData, unsigned short len){}`中的数组变量`tAxit`；

可以通过函数`int axit_get(Axis_t *axit);`获取数据。
# ROS系统下使用触觉传感器(RS485)
## 1. 环境要求
本例程中使用Cmake组织工程，在Ubuntu 20.04 LTS下编译通过，使用ROS1操作系统。

新建工作空间并初始化：

`mkdir -p ~/catkin_ws/src`

`cd ~/catkin_ws/src`

`catkin_init_workspace`

`cd ~/catkin_ws/`

`catkin_make`

#如果使用conda环境，可以使用如下指令进行编译：

#if in conda env,use this command instead:

`catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3`

#将ros软件包tactile_driver拷贝到工作空间src目录下
#copy tactile_driver directory into src floder

#安装rosserial软件包

#install rosserial package

`sudo apt-get install ros-noetic-serial  #ros为Kinect版本`
OR
`sudo apt-get install ros-melodic-serial  #ros为melodic版本`

`cd ~/catkin_ws && catkin_make`

`source devel/setup.bash`

#read serial

`ls /dev/ttyACM0`
OR
`ls /dev/ttyUSB0`

`sudo chmod 777 /dev/ttyACM0`
OR
`sudo chmod 777 /dev/ttyUSB0`

#启动带UI可视化界面的launch文件

*（Tips:显示效果可依据需求进行参数更改）*

#launch with UI

*(Tips:The display effect can be changed by revising the parameters)*


`roslaunch tactile_driver demo.launch`

单独运行串口数据读取节点

Run the serial port data reading node independently

`rosrun tactile_driver serial_read`

传感器触点数据会发布在"/tactile_data"的话题中，默认发布频率为10Hz

# windos平台下使用触觉传感器(RS485)
## 1. 下载并安装MinGW
下载地址：https://sourceforge.net/projects/mingw/files/

安装，勾选mingw32-base和mingw32-gcc-g++

或参考如下教程
https://blog.csdn.net/qq_38196449/article/details/136125995

## 2. 安装gcc后确认是否安装成功

`gcc --version`

输出类似的信息

`gcc.exe (x86_64-win32-sjlj-rev0, Built by MinGW-W64 project) 8.1.0 Copyright (C) 2018 Free Software Foundation, Inc. This is free software; see the source for copying conditions.  There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.`

## 3. 编译并运行工程，获取原始16进制数据
查看设备对应的端口号 eg. COM4
将工程放入不包含中文字符的路径中。
修改hexraw.cpp文件中 const char* portName = "COM4"; // 串口名称  


`mkdir build`

`cd build`

`cmake ..`

`cmake --build .`

`cd \Debug\test`

`.\hexraw.exe`

得到如下所示的数据输出：
![16进制原始数据](./pictures/hexrawOutput.png)

## 4. 输出传感器数据
to be continue...

# windos平台下使用触觉传感器(CAN)