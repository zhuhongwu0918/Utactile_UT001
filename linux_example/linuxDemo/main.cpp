#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include "Communication/Inc/app.h"
#include "Communication/Inc/decode_usart.h"
/*
待修改
1.将配置写到主函数
2.使用函数直接获取数组数据
*/

int main(int argc, char *argv[])
{
    int ret;
    ret = demo_app_init();
    if(ret)
    {
        exit(EXIT_FAILURE);
    }
    printf("skin_demo run\n");
    while(1);
    //通过该函数打印数据 void axit_decode(unsigned char *pData, unsigned short len)
    //存放在tAxit数组中
    sleep(1);
    /* 退出 */
    demo_app_exit();
}

