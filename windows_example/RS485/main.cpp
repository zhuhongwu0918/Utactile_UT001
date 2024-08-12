#include <iostream>  
#include <iomanip>  
#include <windows.h>  
#include "uartconfig.h"
  
int main(int argc, char* argv[]) 
{  
    Axis_t tAxit[SENSOR_NUMBER];
    int res;

    if(argc < 2)
    {
        std::cout<<"Please input the serial device name such as: main.exe COM5"<<std::endl;
        return 0;
    }

    //初始化串口
    res = uart_init(argv[1]);
    if(res)
    {
        std::cout<<"uart init fail"<<std::endl;
        return 0;
    }

    //设置传感器的采样频率
    sensor_freq_sample_set(SAMPLE_FREQ_20HZ);
    //设置传感器输出的数据类型，默认为力数据
    sensor_output_set(OUTPUT_TYPE_FORCE);
    while(1)
    {
        res = axit_get(tAxit);
        if(res != 0)                //获取到数据
        {
            for(int i = 0; i < res; i++)
            {
                std::cout<<"sensor"<<i<<":"<<tAxit[i].x<<"\t"<<tAxit[i].y<<"\t"<<tAxit[i].z<<std::endl;
            }
            std::cout<<std::endl;
        }
    }

    uart_exit();
    return 0;
}