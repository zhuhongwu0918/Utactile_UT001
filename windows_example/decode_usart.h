/*
 * decode_usart.h
 *
 *  Created on: Apr 25, 2024
 *      Author: Jinming
 */

#ifndef INC_DECODE_USART_H_
#define INC_DECODE_USART_H_

#define USART_RX_BUFFER_SIZE		256
#define USART_TX_CMD_COUNT			10
#define USART_TX_BUFFER_SIZE		150

typedef union{
	struct
	{
		unsigned char header;
		unsigned char len;
		unsigned char type;
		unsigned char data[0];
	};

	unsigned char buf[USART_TX_BUFFER_SIZE];
}UsartTxFormat_t;

typedef struct
{
	unsigned char aBuffer[USART_RX_BUFFER_SIZE];          //用来存储帧数据的buffer
	unsigned short hPos;            //用来指示当前完整帧的位置，为0时表示当前没有识别到帧数据
	unsigned short hFrameLen;             //记录当前帧的帧长度
	unsigned short hBufDataLen;
}UsartRxDecode_t;

typedef void (*decodeOkdef)(unsigned char *pData, unsigned short len);

void decode_usart_loop(void);
int decode_usart_init(decodeOkdef ok, decodeOkdef fail, void (*send)(unsigned char *pData, unsigned short Size));
int usart_send_buffer(unsigned char type, unsigned char *pData, unsigned short len);
int usb_rx_data_decode(unsigned char *pData, unsigned short len);
int usart_rx_data_decode(unsigned char *pData, unsigned short len);

#endif /* INC_DECODE_USART_H_ */
