/************************************************************
 * CAN数据编解码文件，CAN ID组成如下：
 * ------------------------------------
 * | CAN ID bits | [10]~[3] | [2]~[0] |
 * ------------------------------------
 * |   功能定义   | 命令类型  | 设备ID  |
 * ------------------------------------
*********************************************************/
#include <string.h>
#include "decode_can.h"
#include "command_type.h"

DecodeCAN::DecodeCAN()
{
    ;
}

//获取接收数据信息
void DecodeCAN::getAxitInfo(Axis_t *axit)
{
    memcpy(axit, &tAxit[iAxitReadPos], sizeof(Axis_t));
    iAxitReadPos++;
    if(iAxitReadPos >= AXIT_BUFFER_LEN)
    {
        iAxitReadPos = 0;
    }
}

//解码
void DecodeCAN::axit_decode(unsigned int devId, unsigned char *pData, unsigned short len)
{
    tAxit[iAxitWritePos].devId = devId;
    tAxit[iAxitWritePos].sensorId = pData[0];
    tAxit[iAxitWritePos].x = pData[1];
    tAxit[iAxitWritePos].x <<= 8;
    tAxit[iAxitWritePos].x |= pData[2];

    tAxit[iAxitWritePos].y = pData[3];
    tAxit[iAxitWritePos].y <<= 8;
    tAxit[iAxitWritePos].y |= pData[4];

    tAxit[iAxitWritePos].z = pData[5];
    tAxit[iAxitWritePos].z <<= 8;
    tAxit[iAxitWritePos].z |= pData[6];

    // std::cout<<"devId:"<<(int)tAxit[iAxitWritePos].devId<<"\tindex:"<<(int)tAxit[iAxitWritePos].sensorId<<":" \
    //     <<tAxit[iAxitWritePos].x<<"\t"<<tAxit[iAxitWritePos].y<<"\t"<<tAxit[iAxitWritePos].z<<std::endl;


    iAxitWritePos++;
    if(iAxitWritePos >= AXIT_BUFFER_LEN)
    {
        iAxitWritePos = 0;
    }

    recNotify();
}

int DecodeCAN::txDataEncode(unsigned char command, unsigned char *pData, unsigned short len)
{
    TxFormat_t txData;

    txData.id = command;
    txData.id <<= 3;
    txData.id |= CAN_ID;

    int iLen = (len > 8) ? 8 : len;
    txData.len = iLen;
    memcpy(txData.data, pData, iLen);
    return send(&txData);
}

//CAN帧解码
int DecodeCAN::can_rx_data_decode(unsigned int Id, unsigned char *pData, unsigned short len)
{
    unsigned char devId = Id & 0x7;
    int command = (Id >> 3) & 0xff;

    if(devId != CAN_ID)
        return -1;

    switch (command)
    {
    case SENSOR_CHECKED_DATA:
        axit_decode(devId, pData, len);
        break;

    case SENSOR_ORIGIN_DATA:
        axit_decode(devId, pData, len);
        break;
    
    default:
        break;
    }

    return 0;
}


