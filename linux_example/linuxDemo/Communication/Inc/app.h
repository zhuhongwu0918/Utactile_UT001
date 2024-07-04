/*
 * usart_protocol.h
 *
 *  Created on: Sep 7, 2023
 *      Author: Jinming
 */

#ifndef INC_APP_H_
#define INC_APP_H_


#define SENSOR_NUMBER	4

typedef struct
{
	short x;
	short y;
	short z;
}Axis_t;

int demo_app_init(void);
void demo_app_exit(void);
int axit_get(Axis_t *axit);

#endif /* INC_APP_H_ */
