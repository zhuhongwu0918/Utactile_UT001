#ifndef _USB_CAN_H__
#define _USB_CAN_H__

#include <iostream>
#include "decode_can.h"

using namespace std;

#define SENSOR_NUMBER	4

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

class CANAlyst : public DecodeCAN
{
public:
    CANAlyst();

    void recNotify();                   //虚函数的重写
    int send(TxFormat_t *txData);      //虚函数的重写

    int UsbCanInit(int devType, int devId, int channel);
    void UsbCanExit(void);
    int getAxit(Axis_t *axit);
    int sensor_freq_sample_set(unsigned char freqIndex);
    int sensor_output_set(unsigned char type);

private:
    int iDeviceType = 4; /* USBCAN-2A或USBCAN-2C或CANalyst-II */
    int iDeviceInd = 0; /* 第1个设备 */
    int iCANInd = 1;            /* 第1个通道 */
    bool bThreadExit;
    volatile HANDLE tListenThread;
    HANDLE tSem;

    static unsigned int __stdcall ListenThread(void* pParam);
    int CloseListenTread();
    int OpenListenThread();
};


#endif   //_USB_CAN_H__
