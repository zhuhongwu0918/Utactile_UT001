#include <iostream>  
#include <iomanip>  
#include <windows.h>
#include "usb_can.h"  

int main(int argc, char* argv[])
{
    int ret;
    Axis_t tAxit;
    CANAlyst tCANAlyst;
    int channel;

    if(argc < 2)
    {
        std::cout<<"Please select Device channel such as : main.exe CAN1"<<std::endl;
        return -1;
    }

    if(strcmp(argv[1], "CAN1") == 0)
    {
        channel = 0;
    }
    else if(strcmp(argv[1], "CAN2") == 0)
    {
        channel = 1;
    }
    else
    {
        std::cout<<"Device channel input error"<<std::endl;
        std::cout<<"Please select Device channel such as : main.exe CAN1"<<std::endl;
        return -1;
    }

    //初始化CAN分析仪
    ret = tCANAlyst.UsbCanInit(4, 0, channel);
    if(ret)
    {
        std::cout<<"USB CAN init fail"<<std::endl;
        return -1;
    }

    //设置传感器的采样频率
    tCANAlyst.sensor_freq_sample_set(SAMPLE_FREQ_50HZ);
    Sleep(10);
    //设置传感器输出的数据类型，默认为力数据
    tCANAlyst.sensor_output_set(OUTPUT_TYPE_FORCE);
    while(1)
    {
        //获取接收的三轴数据
        ret = tCANAlyst.getAxit(&tAxit);
        if(ret == 0)        //接收到数据
        {
            std::cout<<"devId:"<<(int)tAxit.devId<<"\tindex:"<<(int)tAxit.sensorId<<":" \
                <<tAxit.x<<"\t"<<tAxit.y<<"\t"<<tAxit.z<<std::endl;
        }
    }

    //退出
    tCANAlyst.UsbCanExit();

    return 0;
}