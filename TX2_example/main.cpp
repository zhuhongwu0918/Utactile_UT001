#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "can_config.h"


int main(int argc, char *argv[])
{
    int ret;
    Axis_t tAxit;
    CANConfig canConfig;
    struct timeval tv;

    if(argc < 2)
    {
        printf("CAN device select error,please try again");
        exit(EXIT_FAILURE);
    }

    ret = canConfig.init(argv[1]);
    if(ret)
    {
        exit(EXIT_FAILURE);
    }

    //设置传感器的采样频率
    canConfig.sensor_freq_sample_set(SAMPLE_FREQ_20HZ);
    //设置传感器输出的数据类型，默认为力数据
    canConfig.sensor_output_set(OUTPUT_TYPE_FORCE);
    while(1)
    {
        //实时读取传感器输出的数据
        canConfig.getAxit(&tAxit);
        gettimeofday(&tv, NULL);
        printf("[%lu]devId:%d\tindex:%d\tx:%d\ty:%d\tz:%d\n", tv.tv_sec * 1000 + tv.tv_usec / 1000,  \
            tAxit.devId, tAxit.sensorId, tAxit.x, tAxit.y, tAxit.z);
    }
    
    /* 退出 */
    canConfig.exit();
    exit(EXIT_SUCCESS);
}

