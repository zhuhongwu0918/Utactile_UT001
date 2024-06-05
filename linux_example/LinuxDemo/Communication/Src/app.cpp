/*
 * App.c
 * 与主机通信的协议响应
 *  Created on: Sep 7, 2023
 *      Author: Jinming
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include "app.h"
#include "decode_usart.h"
#include "command_type.h"

#define TAG					"Protocol"

static unsigned char gs_tSendBuf[100];
Axis_t tAxit[SENSOR_NUMBER];

/***************************以下部分为USB串口配置部分*****************************/
typedef struct uart_hardware_cfg {
    unsigned int baudrate;          /* 波特率 */
    unsigned char dbit;             /* 数据位 */
    char parity;                    /* 奇偶校验 */
    unsigned char sbit;             /* 停止位 */
} uart_cfg_t;

static struct termios old_cfg;      //用于保存终端的配置参数
static int fd;                      //串口终端对应的文件描述符

/******************************************************************************
 * @brief 	串口初始化操作
 * @param	device : 表示串口终端的设备节点
 * @retval  0：初始化成功 -1：初始化失败
*******************************************************************************/
static int uart_init(const char *device) {
    /* 打开串口终端 */
    fd = open(device, O_RDWR | O_NOCTTY);
    if (0 > fd) {
        fprintf(stderr, "open error: %s: %s\n", device, strerror(errno));
        return -1;
    }

    /* 获取串口当前的配置参数 */
    if (0 > tcgetattr(fd, &old_cfg)) {
        fprintf(stderr, "tcgetattr error: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    return 0; 
}

/******************************************************************************
 * @brief 	串口配置
 * @param	cfg : 指向一个 uart_cfg_t 结构体对象
 * @retval  0：配置成功
*******************************************************************************/
static int uart_cfg(const uart_cfg_t *cfg) {
    struct termios new_cfg = {0}; //将 new_cfg 对象清零
    speed_t speed;
    /* 设置为原始模式 */
    cfmakeraw(&new_cfg);
    /* 使能接收 */
    new_cfg.c_cflag |= CREAD;
    /* 设置波特率 */
    switch (cfg->baudrate) {
        case 1200: speed = B1200;
            break;
        case 1800: speed = B1800;
            break;
        case 2400: speed = B2400;
            break;
        case 4800: speed = B4800;
            break;
        case 9600: speed = B9600;
            break;
        case 19200: speed = B19200;
            break;
        case 38400: speed = B38400;
            break;
        case 57600: speed = B57600;
            break;
        case 115200: speed = B115200;
            break;
        case 230400: speed = B230400;
            break;
        case 460800: speed = B460800;
            break;
        case 500000: speed = B500000;
            break;
        default: //默认配置为 115200
            speed = B115200;
            printf("default baud rate: 115200\n");
            break;
    }

    if (0 > cfsetspeed(&new_cfg, speed)) {
        fprintf(stderr, "cfsetspeed error: %s\n", strerror(errno));
        return -1;
    }
    /* 设置数据位大小 */
    new_cfg.c_cflag &= ~CSIZE; //将数据位相关的比特位清零
    switch (cfg->dbit) {
        case 5:
            new_cfg.c_cflag |= CS5;
            break;
        case 6:
            new_cfg.c_cflag |= CS6;
            break;
        case 7:
            new_cfg.c_cflag |= CS7;
            break;
        case 8:
            new_cfg.c_cflag |= CS8;
            break;
        default: //默认数据位大小为 8
            new_cfg.c_cflag |= CS8;
            printf("default data bit size: 8\n");
            break;
    }

    /* 设置奇偶校验 */
    switch (cfg->parity) {
        case 'N': //无校验
        new_cfg.c_cflag &= ~PARENB;
        new_cfg.c_iflag &= ~INPCK;
        break;
    case 'O': //奇校验
        new_cfg.c_cflag |= (PARODD | PARENB);
        new_cfg.c_iflag |= INPCK;
        break;
    case 'E': //偶校验
        new_cfg.c_cflag |= PARENB;
        new_cfg.c_cflag &= ~PARODD; /* 清除 PARODD 标志，配置为偶校验 */
        new_cfg.c_iflag |= INPCK;
        break;
    default: //默认配置为无校验
        new_cfg.c_cflag &= ~PARENB;
        new_cfg.c_iflag &= ~INPCK;
        printf("default parity: N\n");
        break;
    }


    /* 设置停止位 */
    switch (cfg->sbit) {
        case 1: //1 个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            break;
        case 2: //2 个停止位
            new_cfg.c_cflag |= CSTOPB;
            break;
        default: //默认配置为 1 个停止位
            new_cfg.c_cflag &= ~CSTOPB;
            printf("default stop bit size: 1\n");
            break;
    }


    /* 将 MIN 和 TIME 设置为 0 */
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 0;

    /* 清空缓冲区 */
    if (0 > tcflush(fd, TCIOFLUSH)) {
        fprintf(stderr, "tcflush error: %s\n", strerror(errno));
        return -1;
    }

    /* 写入配置、使配置生效 */
    if (0 > tcsetattr(fd, TCSANOW, &new_cfg)) {
        fprintf(stderr, "tcsetattr error: %s\n", strerror(errno));
        return -1;
    }

    /* 配置 OK 退出 */
    return 0; 
 }

/******************************************************************************
 * @brief 	信号处理函数，当串口有数据可读时，会跳转到该函数执行
 * @param	sig : 信号
 * @param	info: 详细信息
 * @param   context
 * @retval
*******************************************************************************/
static void io_handler(int sig, siginfo_t *info, void *context) {
    unsigned char buf[10] = {0};
    int ret;
    if(SIGRTMIN != sig)
        return;

    /* 判断串口是否有数据可读 */
    if (POLL_IN == info->si_code) {
        ret = read(fd, buf, 8);     //一次最多读 8 个字节数据
        usart_rx_data_decode(buf, ret);
    } 
}

/******************************************************************************
 * @brief 	异步 I/O 初始化函数
 * @retval
*******************************************************************************/
static void async_io_init(void) {
    struct sigaction sigatn;
    int flag;

    /* 使能异步 I/O */
    flag = fcntl(fd, F_GETFL); //使能串口的异步 I/O 功能
    flag |= O_ASYNC;
    fcntl(fd, F_SETFL, flag);

    /* 设置异步 I/O 的所有者 */
    fcntl(fd, F_SETOWN, getpid());

    /* 指定实时信号 SIGRTMIN 作为异步 I/O 通知信号 */
    fcntl(fd, F_SETSIG, SIGRTMIN);

    /* 为实时信号 SIGRTMIN 注册信号处理函数 */
    sigatn.sa_sigaction = io_handler; //当串口有数据可读时，会跳转到 io_handler 函数
    sigatn.sa_flags = SA_SIGINFO;
    sigemptyset(&sigatn.sa_mask);
    sigaction(SIGRTMIN, &sigatn, NULL);
}

void demo_app_exit(void)
{
	tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
    close(fd);
    exit(EXIT_SUCCESS);
}


/*****************************以下部分为协议响应部分******************************/

/******************************************************************************
 * @brief 	三维受力数据解析
 * @param	pData: 待解析数据
 * @param	len	 : 带解析数据长度
 * @retval
*******************************************************************************/
void axit_decode(unsigned char *pData, unsigned short len)
{
    unsigned short sensorCount = len / 6;
    unsigned short pos = 0;
	short tmp;

	memset(tAxit, 0, SENSOR_NUMBER * sizeof(Axis_t));
    for(unsigned int i = 0; i < sensorCount; i++)
    {
        pos = 6 * i;
		tAxit[i].x = ((short)pData[pos] << 8)|pData[pos + 1];
        tAxit[i].y = ((short)pData[pos + 2] << 8)|pData[pos + 3];
        tAxit[i].z = ((short)pData[pos + 4] << 8)|pData[pos + 5];
		printf("sensor%d:\tx:%d\ty:%d\tz:%d\r\n", i, tAxit[i].x, tAxit[i].y, tAxit[i].z);
    }
	printf("\r\n");

    /* tAxit变量为每个传感器的三轴数据，此处可添加用户代码 */
}

/******************************************************************************
 * @brief 	参数发送接口
 * @param	type: 指令类型
 * @param	argc: 参数数量
 * @retval  0：发送成功，非0：发送失败
*******************************************************************************/
int usart_send_args(unsigned char type, int argc, ...)
{
	unsigned char buffer[5];
	va_list argv;

	va_start(argv,argc);
	for (unsigned char i = 0; i < argc; i++)
	{
		buffer[i] = (unsigned char)va_arg(argv, int);
	}
	va_end(argv);//结束标志
/*******************************串口发送**********************************************/
	usart_send_buffer(type, buffer, argc);

	return 0;
}

/******************************************************************************
 * @brief 	数据解析失败回调接口
 * @param	pData: 解析失败数据
 * @param	len:   解析失败数据长度
 * @retval
******************************************************************************/
static void decode_fail(unsigned char *pData, unsigned short len)
{
	;
}

/******************************************************************************
 * @brief 	数据解析成功回调接口
 * @param	pData: 解析成功数据
 * @param	len:   解析成功数据长度
 * @retval
******************************************************************************/
static void decode_ok(unsigned char *pData, unsigned short len)
{
	switch(pData[0])
	{
	case SENSOR_CHECKED_DATA:
		axit_decode(&pData[1], len);
		break;

	case SENSOR_ORIGIN_DATA:
		break;

	default:
		break;
	}
}

/******************************************************************************
 * @brief 	用户获取三轴受力数据的接口
 * @param	axit: 用户传入的数据结构缓冲区
 * @retval  传感器数量
******************************************************************************/
int axit_get(Axis_t *axit)
{
	memcpy(axit, tAxit, SENSOR_NUMBER * sizeof(Axis_t));
	return SENSOR_NUMBER;
}

/******************************************************************************
 * @brief 	数据采样频率设置
 * @param	freqIndex: 采样频率索引，0~4分别对应10、20、50、100、200Hz采样频率
 * @retval
*******************************************************************************/
void app_freq_sample_set(unsigned char freqIndex)
{
	usart_send_args(HOST_SENSOR_SCAN_FRE_SET, 1, freqIndex);
}

/******************************************************************************
 * @brief 	用户USART发送接口，用于初始化传入协议文件
 * @param	pData: 发送数据
 * @param	len:   发送长度长度
 * @retval
******************************************************************************/
void demo_send(unsigned char *pData, unsigned short Size)
{
	write(fd, pData, Size); //一次向串口写入 8 个字节
}

/******************************************************************************
 * @brief 	用户初始化
 * @retval
******************************************************************************/
int demo_app_init(void)
{
	uart_cfg_t cfg = {0};
    char device[] = "/dev/ttyUSB0";
    int rw_flag = -1;

	cfg.baudrate = 115200;
    cfg.dbit = 8;
    cfg.parity = 'N';
    cfg.sbit = 1;
    rw_flag = 0; //读

	decode_usart_init(decode_ok, decode_fail, demo_send);

 	/* 串口初始化 */
	printf("串口初始化 \n");
    if (uart_init(device))
	{
        return -1;
	}

	/* 串口配置 */
    printf("串口配置 \n");
    if (uart_cfg(&cfg)) 
	{
        tcsetattr(fd, TCSANOW, &old_cfg); //恢复到之前的配置
        close(fd);
        return -1;
    }
	async_io_init();

	return 0;
}

