#include <Windows.h>
#include <windows.h>
#include <process.h>
#include "ControlCAN.h"
#include "usb_can.h"
#include "command_type.h"

CANAlyst::CANAlyst()
{
    ;
}

//CAN监听线程
unsigned int __stdcall CANAlyst::ListenThread(void* pParam)
{
    DWORD dwRel;
    VCI_CAN_OBJ vco[2500];
    CANAlyst* pCANAlyst = reinterpret_cast<CANAlyst*>(pParam);

	// 线程循环,轮询方式读取串口数据  
	while (!pCANAlyst->bThreadExit)
	{
        dwRel = VCI_Receive(pCANAlyst->iDeviceType, pCANAlyst->iDeviceInd, pCANAlyst->iCANInd, vco, 2500, 0);
        if(dwRel > 0)
        {
            std::cout<<"rev:"<<dwRel<<std::endl;
            for(int i = 0; i < dwRel; i++)
            {
                pCANAlyst->can_rx_data_decode(vco[i].ID, vco[i].Data, vco[i].DataLen);
            }
        }
        Sleep(2);
	}
 
	return 0;
}

//打开监听线程
int CANAlyst::OpenListenThread()
{
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
        std::cout << "Setting priority of thread error" << std::endl;
		return -1;
	}

    tSem = CreateSemaphore(NULL, 0, 10, NULL);
    if (tSem == NULL) 
    {
        std::cout << "Create Sem error" << std::endl;
        return -1;
    }

    std::cout<<"Thread create ok"<<std::endl;
	return 0;
}

//关闭监听线程
int CANAlyst::CloseListenTread()
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

void CANAlyst::recNotify()
{
    ReleaseSemaphore(tSem, 1, NULL);
}

int CANAlyst::send(TxFormat_t *txData)
{
    VCI_CAN_OBJ vco;

    vco.ID = txData->id;
    vco.RemoteFlag = 0;
    vco.ExternFlag = 0;
    vco.DataLen = txData->len;
    for(int i = 0; i < txData->len; i++)
    {
        vco.Data[i] = txData->data[i];
    }

    // std::cout<<"ID:"<<vco.ID<<std::endl;
    // std::cout<<"len:"<<(int)vco.DataLen<<"\t"<<(int)vco.Data[0]<<::endl;
    DWORD dwRel = VCI_Transmit(iDeviceType, iDeviceInd, iCANInd, &vco, 1);
    // std::cout<<"transmit cout:"<<dwRel<<std::endl;
    return dwRel;
}

/******************************************************************************
 * @brief 	数据采样频率设置
 * @param	freqIndex: 采样频率索引，0~4分别对应10、20、50、100、200Hz采样频率
 * @retval  实际发送的帧数, -1表示USB-CAN设备不存在或USB掉线
*******************************************************************************/
int CANAlyst::sensor_freq_sample_set(unsigned char freqIndex)
{
	return txDataEncode(HOST_SENSOR_SCAN_FRE_SET, &freqIndex, 1);
}

/******************************************************************************
 * @brief 	数据输出类型设置
 * @param	type: 数据类型
 *              0：力数据 
 *              1：源数据
 * @retval 实际发送的帧数, -1表示USB-CAN设备不存在或USB掉线
*******************************************************************************/
int CANAlyst::sensor_output_set(unsigned char type)
{
	return txDataEncode(HOST_SENSOR_DATA_TYPE_SET, &type, 1);
}

/******************************************************************************
 * @brief 	初始化CAN总线，并开启监听
 * @param	devType: 设备类型
 * @param   devId：设备索引，由CAN分析仪插入的顺序决定
 * @param   channel：CAN1或者CAN2
 * @retval 实际发送的帧数, -1表示USB-CAN设备不存在或USB掉线
*******************************************************************************/
int CANAlyst::UsbCanInit(int devType, int devId, int channel)
{
    DWORD dwRel;
    VCI_INIT_CONFIG vic;
    VCI_BOARD_INFO vbi;

    iDeviceType = devType; /* USBCAN-2A或USBCAN-2C或CANalyst-II */
    iDeviceInd = devId; /* 第1个设备 */
    iCANInd = channel;            /* 第1个通道 */

    dwRel = VCI_OpenDevice(iDeviceType, iDeviceInd, 0);
    if(dwRel != 1)
    {
        std::cout<<"open USB CAN device fail"<<std::endl;
        return -1;
    }

    vic.AccCode=0x80000008;
    vic.AccMask=0xFFFFFFFF;
    vic.Filter=1;
    vic.Timing0=0x00;
    vic.Timing1=0x14;
    vic.Mode=0;
    dwRel = VCI_InitCAN(iDeviceType, iDeviceInd, iCANInd, &vic);
    if(dwRel != 1)
    {
        VCI_CloseDevice(iDeviceType, iDeviceInd);
        std::cout<<"Init CAN Bus fail"<<std::endl;
        return -1;
    }

    dwRel = VCI_ReadBoardInfo(iDeviceType, iDeviceInd, &vbi);
    if(dwRel != 1)
    {
        std::cout<<"Getting Board info error"<<std::endl;
        return -1;
    }
    std::cout<<"Hardware Version:"<<vbi.hw_Version<<std::endl;
    std::cout<<"Firmware Version:"<<vbi.fw_Version<<std::endl;
    std::cout<<"Driver Version:"<<vbi.dr_Version<<std::endl;
    std::cout<<"Interface Version:"<<vbi.in_Version<<std::endl;
    std::cout<<"Serial number:"<<vbi.str_Serial_Num<<std::endl;
    std::cout<<"Hardware Type:"<<vbi.str_hw_Type<<std::endl;

    if(VCI_ClearBuffer(iDeviceType, iDeviceInd, iCANInd) != 1)
    {
        VCI_CloseDevice(iDeviceType, iDeviceInd);
        std::cout<<"Clear buffer error"<<std::endl;
        return -1;
    }

    if(VCI_StartCAN(iDeviceType, iDeviceInd, iCANInd) != 1)
    {
        VCI_CloseDevice(iDeviceType, iDeviceInd);
        std::cout<<"USB CAN start error"<<std::endl;
        return -1;
    }

    //开启接收线程
    if(OpenListenThread())
    {
        VCI_CloseDevice(iDeviceType, iDeviceInd);
        std::cout<<"Open listen thread error"<<std::endl;
        return -1;
    }
    
    return 0;
}

//退出CAN示例程序
void CANAlyst::UsbCanExit(void)
{
    CloseListenTread();
    CloseHandle(tSem);
    VCI_CloseDevice(iDeviceType, iDeviceInd);
}

int CANAlyst::getAxit(Axis_t *axit)
{
    DWORD dwWaitResult = WaitForSingleObject(tSem, INFINITE);
    // 检查等待结果
    if (dwWaitResult == WAIT_OBJECT_0) 
    {
        getAxitInfo(axit);
        return 0;
    }

    return -1;
}



