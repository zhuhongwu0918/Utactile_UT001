#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include "Communication/Inc/app.h"
#include "Communication/Inc/decode_usart.h"


int main(int argc, char *argv[])
{
    int ret;
    Axis_t tAxit[SENSOR_NUMBER];
    ret = demo_app_init("/dev/ttyUSB0");
    if(ret)
    {
        exit(EXIT_FAILURE);
    }
    printf("skin_demo run\n");

    //设置传感器的采样频率
    sensor_freq_sample_set(SAMPLE_FREQ_20HZ);
    //设置传感器输出的数据类型，默认为力数据
    sensor_output_set(OUTPUT_TYPE_FORCE);
    while(1)
    {
        //实时读取传感器输出的数据
        axit_get(tAxit);
        for(int i = 0; i < SENSOR_NUMBER; i++)
        {
            printf("sensor%d:\tx:%d\ty:%d\tz:%d\r\n", i, tAxit[i].x, tAxit[i].y, tAxit[i].z);
        }
        printf("\n");
    }
    
    /* 退出 */
    demo_app_exit();
    exit(EXIT_SUCCESS);
}

