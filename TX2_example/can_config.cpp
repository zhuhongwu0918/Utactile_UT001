#include "can_config.h"
#include "command_type.h"

CANConfig::CANConfig()
{
    ;
}

CANConfig::~CANConfig()
{
	close(devHandle);
}

int CANConfig::init(char *dev)
{
	int ret;
    struct can_filter rfilter[1];

	devHandle = socket(PF_CAN, SOCK_RAW, CAN_RAW); //创建套接字
	if(devHandle < 0)
	{
		perror("Open socket error");
		return -1;
	}
	strcpy(ifr.ifr_name, dev);
	ioctl(devHandle, SIOCGIFINDEX, &ifr); //指定 can0 设备
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	ret = bind(devHandle, (struct sockaddr *)&addr, sizeof(addr)); //将套接字与 can0 绑定
	if(ret < 0)
	{
		perror("CAN bind error");
		return -1;
	}
	// //定义接收规则,只接收表示符等于 0x11 的报文
	// rfilter[0].can_id = 0x11;
	// rfilter[0].can_mask = CAN_SFF_MASK;
	// //设置过滤规则
	// setsockopt(devHandle, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	// 初始化信号量
    sem_init(&tSem, 0, 0);

	// 创建线程
    ret = pthread_create(&tSensorThread, NULL, &SensorListenThread, this);
    if (ret != 0) 
    {
        printf("Thread creation failed.\n");
        return -1;
    }
}

void CANConfig::exit(void)
{
    close(devHandle);
}

void* CANConfig::SensorListenThread(void* arg)
{
	int ret;
	struct can_frame frame;
	CANConfig *pCANConfig = (CANConfig *)arg;

	while(1)
	{
		ret = read(pCANConfig->devHandle, &frame, sizeof(frame));
		//接收报文//显示报文
		if(ret > 0)
		{
			pCANConfig->can_rx_data_decode(frame.can_id, frame.data, frame.can_dlc);
			// printf("ID=0x%X DLC=%d data[0]=0x%x\n",frame.can_id, frame.can_dlc, frame.data[0]);
		}
	}
}

void CANConfig::recNotify()                   //虚函数的重写
{
    sem_post(&tSem); // 发送信号量;
}

int CANConfig::send(TxFormat_t *txData)      //虚函数的重写
{
	struct can_frame frame;
	int ret;

    frame.can_id = txData->id;
	frame.can_dlc = txData->len;

	for(int i = 0; i < txData->len; i++)
	{
		frame.data[i] = txData->data[i];
	}

    ret = write(devHandle, &frame, sizeof(frame));
    if(ret != sizeof(frame))
    {
        printf("Send Error %d\n!", ret);
        return -1; 				//发送错误,退出
    }

	return 0;
}

int CANConfig::getAxit(Axis_t *axit)
{
	sem_wait(&tSem); // 线程等待信号量;
	getAxitInfo(axit);
}

int CANConfig::sensor_freq_sample_set(unsigned char freqIndex)
{
	return txDataEncode(HOST_SENSOR_SCAN_FRE_SET, &freqIndex, 1);
}

int CANConfig::sensor_output_set(unsigned char type)
{
	return txDataEncode(HOST_SENSOR_DATA_TYPE_SET, &type, 1);
}

