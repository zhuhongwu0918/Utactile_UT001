/*
 * usart_protocol.h
 *
 *  Created on: Sep 7, 2023
 *      Author: Jinming
 */

#ifndef INC_APP_H_
#define INC_APP_H_


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

/******************************************************************************
 * @brief 	数据采样频率设置
 * @param	freqIndex: 采样频率索引，0~4分别对应10、20、50、100、200Hz采样频率
 * @retval
*******************************************************************************/
void sensor_freq_sample_set(unsigned char freqIndex);

/******************************************************************************
 * @brief 	数据输出类型设置
 * @param	type: 数据类型
 *          OUTPUT_TYPE_FORCE：力数据 
 *          OUTPUT_TYPE_ORIGIN：源数据
 * @retval
*******************************************************************************/
void sensor_output_set(unsigned char type);

/******************************************************************************
 * @brief 	用户初始化
 * @retval
******************************************************************************/
int demo_app_init(const char *dev);

void demo_app_exit(void);

/******************************************************************************
 * @brief 	用户获取三轴受力数据的接口
 * @param	axit: 用户传入的数据结构缓冲区
 * @retval  传感器数量
******************************************************************************/
int axit_get(Axis_t *axit);

#endif /* INC_APP_H_ */
