#ifndef _DECODE_CAN_H__
#define _DECODE_CAN_H__

#include <iostream>
#include "command_type.h"

#define CAN_ID      0x4             //定义设备的CAN ID
#define AXIT_BUFFER_LEN     50

typedef struct
{
    unsigned char devId;        //设备ID/CAN ID
    unsigned char sensorId;     //传感器ID
	short x;
	short y;
	short z;
}Axis_t;

typedef struct
{
    unsigned int id;
    unsigned char data[8];
    unsigned char len;
}TxFormat_t;        //发送数据格式规范

class DecodeCAN
{
public:
    DecodeCAN();

    virtual void recNotify(){};           //数据接收通知
    virtual int send(TxFormat_t *txData){ return 0;};

    int txDataEncode(unsigned char command, unsigned char *pData, unsigned short len);
    int can_rx_data_decode(unsigned int Id, unsigned char *pData, unsigned short len);  //解码
    void getAxitInfo(Axis_t *axit);

private:
    int iAxitReadPos = 0;
    int iAxitWritePos = 0;
    Axis_t tAxit[AXIT_BUFFER_LEN];
    void axit_decode(unsigned int devId, unsigned char *pData, unsigned short len);
    
};

#endif
