#ifndef _CAN_CONFIG_H__
#define _CAN_CONFIG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include "decode_can.h"


enum{
    SAMPLE_FREQ_10HZ = 0,
    SAMPLE_FREQ_20HZ,
    SAMPLE_FREQ_50HZ,
    SAMPLE_FREQ_100HZ,
    SAMPLE_FREQ_102HZ,
};

enum
{
	OUTPUT_TYPE_FORCE = 0,			//力数据
	OUTPUT_TYPE_ORIGIN,				//源数据
};

class CANConfig : public DecodeCAN
{
public:
    CANConfig();
    ~CANConfig();

    int init(char *dev);
    void exit(void);
    int getAxit(Axis_t *axit);
    int sensor_freq_sample_set(unsigned char freqIndex);
    int sensor_output_set(unsigned char type);

private:
    int devHandle;
    struct sockaddr_can addr;
	struct ifreq ifr;
    sem_t tSem;
    pthread_t tSensorThread;

    void recNotify();                   //虚函数的重写
    int send(TxFormat_t *txData);      //虚函数的重写

    static void* SensorListenThread(void* arg);
};

#endif
