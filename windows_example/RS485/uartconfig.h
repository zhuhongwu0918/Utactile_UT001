#ifndef _UART_CONFIG_H__
#define _UART_CONFIG_H__

#include <iostream>
#include <string>
#include <windows.h>
#include <process.h>
#include <string>
#include "decode_usart.h"

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

typedef struct
{
	short x;
	short y;
	short z;
}Axis_t;

class SerialPort
{
public:
    SerialPort();
    ~SerialPort();
    int open(const char *portName, int baudRate);
    int close();
    int read(char* buffer, int bufferSize, int& bytesRead);
    int write(const char* buffer, int bufferSize);
    int OpenListenThread();

private:
    HANDLE tCom;  // 串口句柄
    DCB tSerialParams;  // 串口参数
    COMMTIMEOUTS tTimeout;  // 超时参数
    static bool bThreadExit;
	volatile HANDLE tListenThread;

    int CloseListenTread();
    static unsigned int __stdcall ListenThread(void* pParam);
    UINT GetBytesInCOM();
    int ReadChar(unsigned char *cRecved, int maxLen);
    int64_t timeStampMs();

    CRITICAL_SECTION   m_csCommunicationSync;
};


void sensor_output_set(unsigned char type);
void sensor_freq_sample_set(unsigned char freqIndex);
int axit_get(Axis_t *axit);
int uart_init(const char *comPort);
void uart_exit(void);

#endif //_UART_CONFIG_H__
