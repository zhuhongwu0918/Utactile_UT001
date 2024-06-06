#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include "app.h"
#include "decode_usart.h"

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
    sleep(1);
    /* 退出 */
    demo_app_exit();
}

