/*
 * decode_usart.c
 * 通信协议的编解码
 *  Created on: Apr 25, 2024
 *      Author: Jinming
 */
#include <stdio.h>
#include <string.h>
#include "decode_usart.h"

#define HPHEADER_CODE		0xA5

static UsartRxDecode_t gs_tUsartRxDecode;
static UsartTxFormat_t	g_tTxFormat;
static void (*pFuncUsartSend)(unsigned char *pData, unsigned short Size);

static decodeOkdef decode_ok;
static decodeOkdef decode_fail;

/******************************************************************************
 * @brief 	数据校验
 * @param	pData: 待校验数据
 * @param	len  : 待校验数据长度
 * @retval  校验值
******************************************************************************/
static unsigned char check_sum(unsigned char *pData, unsigned short len)
{
	unsigned char res = 0;

	for(unsigned short i = 0; i < len; i++)
	{
		res += pData[i];
	}

	return res;
}

/******************************************************************************
 * @brief 	寻找头码
 * @param   dev  : 设备解析句柄
 * @param	pData: 待寻找数据
 * @param	len  : 待寻找数据长度
 * @retval  头码位置
******************************************************************************/
int find_header(UsartRxDecode_t *dev, unsigned char *pData, unsigned short len)                 //寻找数据头
{
    int iPos = -1;                      //没找到头码时，返回-1
    for(int i = 0; i < len; i++)
    {
        if(pData[i] == HPHEADER_CODE)        //找到头码
        {
        	dev->hPos = 1;
            iPos = i;
            break;
        }
    }

    return iPos;
}

/******************************************************************************
 * @brief 	数据解析核心函数，解析成功回调接口decode_ok，解析失败回调decode_fail
 * @param   dev  : 设备解析句柄
 * @retval
******************************************************************************/
void redecode(UsartRxDecode_t *dev)
{
    unsigned short hLen;         //记录待处理数据长度
    int iHeaderPos = -1;
    unsigned char cCheckSum = 0;
    unsigned short hFindIndex = 0;

    hLen = dev->hBufDataLen;
    dev->hPos = 0;
    dev->hFrameLen = 0;
    do
    {
        iHeaderPos = find_header(dev, &dev->aBuffer[hFindIndex], hLen - hFindIndex);        //重新寻找头码，防止还有短帧数据在缓冲区
        if(iHeaderPos != -1)                    //找到了头码
        {
            if(hFindIndex + iHeaderPos + 1 == hLen)             //头码位于最后一位数据
            {
            	dev->hPos = 1;
                return;
            }
            dev->hFrameLen = dev->aBuffer[hFindIndex + iHeaderPos + 1];            //寻找新一帧帧尾

            if(dev->hFrameLen < 2)
            {
            	dev->hFrameLen = 0;
                dev->hPos = 0;
                hFindIndex++;
            }
            else if((dev->hFrameLen + 2) > (hLen - hFindIndex - iHeaderPos))                      //剩余数据全部分析完毕，等待下次组帧
            {
                if((hFindIndex + iHeaderPos) > 0)     //该丢弃的数据先做接收解码失败的响应
                {
                    decode_fail(dev->aBuffer, hFindIndex + iHeaderPos);
                }
                memcpy(dev->aBuffer, &dev->aBuffer[hFindIndex + iHeaderPos], hLen - hFindIndex - iHeaderPos);
                dev->hPos = hLen - hFindIndex - iHeaderPos;
                dev->hBufDataLen = dev->hPos;
                return;
            }
            else                                            //可能存在新一帧数据，需要校验解码
            {
                cCheckSum = check_sum(&dev->aBuffer[hFindIndex + iHeaderPos], dev->hFrameLen + 1);
                if(cCheckSum == dev->aBuffer[hFindIndex + iHeaderPos + dev->hFrameLen + 1])     //垃圾堆里找到一帧可用数据
                {
                    if((hFindIndex + iHeaderPos) != 0)      //把失败的数据先响应完
                    {
                        decode_fail(dev->aBuffer, hFindIndex + iHeaderPos);
                    }

                    decode_ok(&dev->aBuffer[hFindIndex + iHeaderPos+ 2], dev->hFrameLen - 1);         //响应新一帧数据

                    hLen = hLen - hFindIndex - iHeaderPos - dev->hFrameLen - 2;                        //新一帧数据响应完毕，重新定义数据长度，并抛弃已经处理完毕的数据
                    memcpy(dev->aBuffer, &dev->aBuffer[hFindIndex + iHeaderPos + dev->hFrameLen + 2], hLen);
                    dev->hBufDataLen = hLen;
                    dev->hPos = 0;
                    dev->hFrameLen = 0;
                    hFindIndex = 0;
                }
                else                        //数据校验有误，重新寻找下一帧
                {
                	dev->hPos = 0;
                	dev->hFrameLen = 0;
                    hFindIndex++;
                    if(hFindIndex >= hLen)          //特殊情况，数据全是A5，数据有误
                    {
                        decode_fail(dev->aBuffer, hFindIndex);
                        return;
                    }
                }
            }
        }
        else
        {
            decode_fail(dev->aBuffer, hLen);
            memset(dev->aBuffer, 0, dev->hBufDataLen);
            hLen = 0;
            dev->hBufDataLen = 0;
        }
    }while(hLen > 0);
}

/******************************************************************************
 * @brief 	用于传入USART总线的接收数据，该函数会调用协议解析函数，并解析出对应的指令
 * @param   pData : 接收数据
 * @param   len   : 接收数据长度
 * @retval
******************************************************************************/
int usart_rx_data_decode(unsigned char *pData, unsigned short len)
{
	int res = 0;

    // printf("len=%d", len);
    memcpy(&gs_tUsartRxDecode.aBuffer[gs_tUsartRxDecode.hBufDataLen], pData, len);
    gs_tUsartRxDecode.hBufDataLen += len;
    redecode(&gs_tUsartRxDecode);

    return res;
}

/******************************************************************************
 * @brief 	通用指令发送函数
 * @param   type  : 指令类型
 * @param   pData : 数据
 * @param   len   : 数据长度
 * @retval  0：发送成功，非0：发送失败
******************************************************************************/
int usart_send_buffer(unsigned char type, unsigned char *pData, unsigned short len)
{
	memset(&g_tTxFormat, 0, sizeof(UsartTxFormat_t));

	g_tTxFormat.header = HPHEADER_CODE;
	g_tTxFormat.len = len + 2;
	g_tTxFormat.type = type;
	memcpy(g_tTxFormat.data, pData, len);
	g_tTxFormat.data[len] = check_sum(g_tTxFormat.buf, g_tTxFormat.len + 1);

	if(pFuncUsartSend)
	{
		pFuncUsartSend(g_tTxFormat.buf, g_tTxFormat.len + 2);			//usart总线串口
	}

	return 0;
}

/******************************************************************************
 * @brief 	协议解析模块初始化
 * @param   ok   : 解析成功时的回调函数
 * @param   fail : 解析失败时的回调函数
 * @param   send : USART发送接口，用于协议需要发送指令时调用
 * @retval  0：初始化成功，非0：初始化失败
******************************************************************************/
int decode_usart_init(decodeOkdef ok, decodeOkdef fail, void (*send)(unsigned char *pData, unsigned short Size))
{
	memset(&gs_tUsartRxDecode, 0, sizeof(UsartRxDecode_t));

	decode_ok = ok;
	decode_fail = fail;
	pFuncUsartSend = send;

	return 0;
}
