#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <time.h>
#include "uartconfig.h"
#include "command_type.h"

using namespace std;

/** 线程退出标志 */
bool SerialPort::bThreadExit = false;
const UINT SLEEP_TIME_INTERVAL = 5;
static HANDLE tSem;
static SerialPort serialPort;
static Axis_t tAxit[SENSOR_NUMBER];

SerialPort::SerialPort() 
{
    tCom = INVALID_HANDLE_VALUE;
    SecureZeroMemory(&tSerialParams, sizeof(DCB));
    SecureZeroMemory(&tTimeout, sizeof(COMMTIMEOUTS));

    //初始化临界区对象。临界区用于同步对串口通信操作的访问。
    InitializeCriticalSection(&m_csCommunicationSync);
}

SerialPort::~SerialPort() 
{
    CloseListenTread();
    close();
    DeleteCriticalSection(&m_csCommunicationSync);
}

int SerialPort::open(const char *portName, int baudRate) 
{
    string prefix = "\\\\.\\";
    prefix.append(portName);
    tCom = CreateFile(prefix.c_str(),            // 串口名称  
                        GENERIC_READ | GENERIC_WRITE, // 读写权限  
                        0,                 // 不允许共享  
                        NULL,              // 安全属性  
                        OPEN_EXISTING,     // 打开已存在的串口  
                        0, // 文件属性  
                        0);              // 不使用模板文件  
  
    if (tCom == INVALID_HANDLE_VALUE) 
    {  
        std::cerr << "Error opening serial port" << std::endl;
	    LeaveCriticalSection(&m_csCommunicationSync);
        return -1;  
    }  

    tSerialParams.DCBlength = sizeof(tSerialParams);
    if (!GetCommState(tCom, &tSerialParams)) 
    {
        std::cout << "Getting uart state fail" << std::endl;
        close();
	    LeaveCriticalSection(&m_csCommunicationSync);
        return -1;
    }

    tSerialParams.BaudRate = baudRate;
    tSerialParams.ByteSize = 8;
    tSerialParams.StopBits = ONESTOPBIT;
    tSerialParams.Parity = NOPARITY;
    if (!SetCommState(tCom, &tSerialParams)) 
    {
        std::cout << "Setting uart state fail" << std::endl;
        close();
	    LeaveCriticalSection(&m_csCommunicationSync);
        return -1;
    }

    tTimeout.ReadIntervalTimeout = 1;       //两个字符之间的最大时间间隔，单位毫秒。
    tTimeout.ReadTotalTimeoutConstant = 1; //指定一个读操作的完成时间的常数部分，单位毫秒。
    tTimeout.ReadTotalTimeoutMultiplier = 1;  //指定一个读操作的完成时间的乘数部分，基于字节数的计算方式。
    tTimeout.WriteTotalTimeoutConstant = 1;    //指定一个写操作的完成时间的常数部分，单位毫秒。
    tTimeout.WriteTotalTimeoutMultiplier = 1;  //指定一个写操作的完成时间的乘数部分，基于字节数的计算方式。
    if (!SetCommTimeouts(tCom, &tTimeout)) 
    {
        std::cout << "Setting uart timeout" << std::endl;
        close();
        LeaveCriticalSection(&m_csCommunicationSync);
        return -1;
    }

    /**  清空串口缓冲区 */
	PurgeComm(tCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

    LeaveCriticalSection(&m_csCommunicationSync);

    return 0;
}

int SerialPort::close() 
{
    if (tCom != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(tCom);
        tCom = INVALID_HANDLE_VALUE;
    }
    return 0;
}

int SerialPort::read(char* buffer, int bufferSize, int& bytesRead) 
{
    if (tCom == INVALID_HANDLE_VALUE) 
        return -1;

    return ReadFile(tCom, buffer, bufferSize, (LPDWORD)&bytesRead, NULL);
}

int SerialPort::write(const char* buffer, int bufferSize)
{
    int bytesWritten = 0;
    bool ret;
    ret = WriteFile(tCom, buffer, bufferSize, (LPDWORD)&bytesWritten, NULL);
    if(ret)
    {
        return bytesWritten;
    }

    return 0;
}

//打开监听线程
int SerialPort::OpenListenThread()
{
	/** 检测线程是否已经开启了 */
	if (tCom == INVALID_HANDLE_VALUE)
	{		
		return -1;      /** 线程已经开启 */
	}
	bThreadExit = false;
	/** 线程ID */
	UINT threadId;

	/** 开启串口数据监听线程 */
	tListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &threadId);
	if (!tListenThread)
	{
        std::cout << "Create listen thread error" << std::endl;
		return -1;
	}
	/** 设置线程的优先级,高于普通线程 */
	if (!SetThreadPriority(tListenThread, THREAD_PRIORITY_ABOVE_NORMAL))
	{
		return -1;
	}

    std::cout<<"Thread create ok"<<std::endl;
	return 0;
}

//关闭监听线程
int SerialPort::CloseListenTread()
{
	if (tListenThread != INVALID_HANDLE_VALUE)
	{
		bThreadExit = true;   /** 通知线程退出 */
		Sleep(10);      /** 等待线程退出 */
		CloseHandle(tListenThread);     /** 置线程句柄无效 */
		tListenThread = INVALID_HANDLE_VALUE;
	}
	return 0;
}

//获取串口缓冲区的字节数
UINT SerialPort::GetBytesInCOM()
{
	DWORD dwError = 0;  /** 错误码 */
	COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
	memset(&comstat, 0, sizeof(COMSTAT));
 
	UINT BytesInQue = 0;
	/** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
	if (ClearCommError(tCom, &dwError, &comstat))
	{
		BytesInQue = comstat.cbInQue; /** 获取在输入缓冲区中的字节数 */
	}
 
	return BytesInQue;
}

//串口监听线程
unsigned int __stdcall SerialPort::ListenThread(void* pParam)
{
	/** 得到本类的指针 */
	SerialPort* pSerialPort = reinterpret_cast<SerialPort*>(pParam);
    unsigned char cRecved[1024];
    int readCount = 0;
 
	// 线程循环,轮询方式读取串口数据  
	while (!pSerialPort->bThreadExit)
	{
        pSerialPort->GetBytesInCOM();
        readCount = pSerialPort->ReadChar(cRecved, sizeof(cRecved));
        if(readCount > 0)
        {
            usart_rx_data_decode(cRecved, readCount);
            // printf("%d:", pSerialPort->timeStampMs());
            // for(int i = 0; i < readCount; i++)
            // {
            //     printf("%x ", cRecved[i]);
            // }
            // printf("\n");
        }
	}
 
	return 0;
}

//读取串口接收缓冲区中一个字节的数据
int SerialPort::ReadChar(unsigned char *cRecved, int maxLen)
{
	BOOL  bResult = TRUE;
	DWORD BytesRead = 0;
	if (tCom == INVALID_HANDLE_VALUE)
	{
		return -1;
	}
 
	/** 临界区保护 */
	EnterCriticalSection(&m_csCommunicationSync);
 
	/** 从缓冲区读取一个字节的数据 */
	bResult = ReadFile(tCom, cRecved, maxLen, &BytesRead, NULL);
	if ((!bResult))
	{
		/** 获取错误码,可以根据该错误码查出错误原因 */
		DWORD dwError = GetLastError();
 
		/** 清空串口缓冲区 */
		PurgeComm(tCom, PURGE_RXCLEAR | PURGE_RXABORT);
		LeaveCriticalSection(&m_csCommunicationSync);
 
		return -1;
	}
 
	/** 离开临界区 */
	LeaveCriticalSection(&m_csCommunicationSync);
 
	return BytesRead;
}

int64_t SerialPort::timeStampMs()
{
	struct timeval tv;

	SYSTEMTIME sys;
	GetLocalTime(&sys);

	struct tm _tm;
	_tm.tm_year = sys.wYear - 1900;
	_tm.tm_mon = sys.wMonth - 1;
	_tm.tm_mday = sys.wDay;
	_tm.tm_hour = sys.wHour;
	_tm.tm_min = sys.wMinute;
	_tm.tm_sec = sys.wSecond;
	_tm.tm_isdst = -1;

	tv.tv_sec = (time_t)mktime(&_tm);
	tv.tv_usec = sys.wMilliseconds * 1000;

	return ((int64_t)tv.tv_sec * 1000 + (int64_t)tv.tv_usec / 1000);
}

/*****************************以下部分为协议响应部分******************************/
void uart_send(unsigned char *pData, unsigned short len)
{
    const char *ptr = reinterpret_cast<const char *>(pData);
    serialPort.write(ptr, len);
}
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
        // 执行完毕后释放信号量
        ReleaseSemaphore(tSem, 1, NULL);
		// printf("sensor%d:\tx:%d\ty:%d\tz:%d\r\n", i, tAxit[i].x, tAxit[i].y, tAxit[i].z);
    }
    // printf("sensor%d:\tx:%d\ty:%d\tz:%d\r\n", 1, tAxit[1].x, tAxit[1].y, tAxit[1].z);
	// printf("\r\n");
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
void decode_fail(unsigned char *pData, unsigned short len)
{
	;
}

/******************************************************************************
 * @brief 	数据解析成功回调接口
 * @param	pData: 解析成功数据
 * @param	len:   解析成功数据长度
 * @retval
******************************************************************************/
void decode_ok(unsigned char *pData, unsigned short len)
{
	switch(pData[0])
	{
	case SENSOR_CHECKED_DATA:
		axit_decode(&pData[1], len);
		break;

	case SENSOR_ORIGIN_DATA:
        axit_decode(&pData[1], len);
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
    DWORD dwWaitResult = WaitForSingleObject(tSem, INFINITE);
    // 检查等待结果，‌如果成功则执行临界区代码
    if (dwWaitResult == WAIT_OBJECT_0) 
    {
        memcpy(axit, tAxit, SENSOR_NUMBER * sizeof(Axis_t));
	    return SENSOR_NUMBER;
    }
	return 0;
}

/******************************************************************************
 * @brief 	数据采样频率设置
 * @param	freqIndex: 采样频率索引，0~4分别对应10、20、50、100、200Hz采样频率
 * @retval
*******************************************************************************/
void sensor_freq_sample_set(unsigned char freqIndex)
{
	usart_send_args(HOST_SENSOR_SCAN_FRE_SET, 1, freqIndex);
}

/******************************************************************************
 * @brief 	数据输出类型设置
 * @param	type: 数据类型
 *              0：力数据 
 *              1：源数据
 * @retval
*******************************************************************************/
void sensor_output_set(unsigned char type)
{
	usart_send_args(HOST_SENSOR_DATA_TYPE_SET, 1, type);
}

void uart_exit(void)
{
    CloseHandle(tSem);
    serialPort.close();
}

int uart_init(const char *comPort)
{
    // 初始化信号量，‌最初可用资源数为1
    tSem = CreateSemaphore(NULL, 0, 1, NULL);
    if (tSem == NULL) 
    {
        return -1;
    }

    if (serialPort.open(comPort, 115200)) 
    {
        std::cout << "Uart open fail" << std::endl;
        return -1;
    }

    std::cout << "Serial port open ok" << std::endl;
    if(serialPort.OpenListenThread())
    {
        std::cout << "open read thread error" << std::endl;
        return -1;
    }
    
    decode_usart_init(decode_ok, decode_fail, uart_send);

    std::cout << "uart init ok" << std::endl;
    return 0;
}
