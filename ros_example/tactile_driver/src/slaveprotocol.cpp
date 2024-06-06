// slaveprotocol.cpp
#include<ros/ros.h>
#include "tactile_driver/slaveprotocol.h"
// #include <QDebug>
// #include "/home/abc/Qt/5.10.0/Src/qtbase/include/QtCore/QDebug"
#include <string.h>
#include<std_msgs/String.h>
#include <tactile_driver/contactpoint.h>  


void SlaveProtocol::rx_timerout_callback(const ros::TimerEvent& event)
{
//    qDebug()<<"rx timer out"<<Qt::endl;
    std::cout<<"rx timer out"<<std::endl;
    if(decode_fail_callback)
    {
        decode_fail_callback(aBuffer, hPos);
    }
    hPos = 0;
    hFrameLen = 0;
    memset(aBuffer, 0, RX_BUFFER_SIZE);
    RxTimer.stop();//
}

char SlaveProtocol::hex2char(unsigned char value)
{
    char res = ' ';
    if(value == 0xF)
    {
        res = 'F';
    }
    else if(value == 0xE)
    {
        res = 'E';
    }
    else if(value == 0xD)
    {
        res = 'D';
    }
    else if(value == 0xC)
    {
        res = 'C';
    }
    else if(value == 0xB)
    {
        res = 'B';
    }
    else if(value == 0xA)
    {
        res = 'A';
    }
    else
    {
        res = value + '0';
    }


    return res;
}

std::string SlaveProtocol::array2hex(unsigned char *pData, unsigned short len)
{
    std::string qstring;

    for(int i = 0; i < len; i++)
    {
        // qstring.append(hex2char((pData[i] >> 4) & 0x0f));
        // qstring.append(hex2char(pData[i] & 0x0f));
        // qstring.append(' ');
        qstring.append(std::string(1, hex2char((pData[i] >> 4) & 0x0f)));
        qstring.append(std::string(1, hex2char(pData[i] & 0x0f)));
        qstring.append(std::string(1, ' '));
    }

    return qstring;
}

SlaveProtocol::SlaveProtocol(ros::NodeHandle &node) : nh_(node)
{
    // RxTimer = nh_.createTimer(ros::Duration(1.0), rx_timerout_callback);
    // connect(RxTimer, SIGNAL(timeout()), this, SLOT(rx_timerout_callback()));
    decode_ok_callback = nullptr;
    decode_fail_callback = nullptr;
    hPos = 0;
    hFrameLen = 0;
//    RxTimer->start(1000);

//    qDebug()<<"Rx timer start"<<Qt::endl;
}

/**********************************************************************************************************
 * function:类构造函数
 * params:解码成功时的回调函数
 * params:解码失败是的回调函数
 * retval:
**********************************************************************************************************/
SlaveProtocol::SlaveProtocol(ros::NodeHandle &node, void (*decode_ok)(unsigned char *pData, unsigned short len), void (*decode_fail)(unsigned char *pData, unsigned short len)) : nh_(node)
{
    // pub_callback = nh_.advertise<std_msgs::String>("/callback_topic", 1);//zhhw
    
    pub_tactile = nh_.advertise<tactile_driver::contactpoint>("/tactile_data", 10);//zhhw
 
    decode_ok_callback = decode_ok;
    decode_fail_callback = decode_fail;

    // RxTimer = new QTimer(this);
    // RxTimer = new ros::Time();
    RxTimer = nh_.createTimer(ros::Duration(1.0), &SlaveProtocol::rx_timerout_callback, this);
    RxTimer.stop();
    // connect(RxTimer, &QTimer::timeout, this, &SlaveProtocol::rx_timerout_callback); //?????
    hPos = 0;
    hFrameLen = 0;
//    RxTimer->start(1000);

//    qDebug()<<"Rx timer start"<<Qt::endl;
}

unsigned char SlaveProtocol::check_sum(unsigned char *pData, unsigned short len)
{
    unsigned char Res = 0;

    for(unsigned short i = 0; i < len; i++)
    {
        Res += pData[i];
    }

    return Res;
}


int SlaveProtocol::find_header(unsigned char *pData, unsigned short len)                 //寻找数据头
{
    int iPos = -1;                      //没找到头码时，返回-1
    for(int i = 0; i < len; i++)
    {
        if(pData[i] == 0xA5)        //找到头码
        {
            aBuffer[hPos] = pData[i];
            hPos = 1;
            iPos = i;
            // RxTimer->start(1000);           //找到了头码，开始倒计时，1s后若没有接收到完整的帧，那么抛弃当前缓冲区所有数据
            break;
        }
    }

    return iPos;
}

/**********************************************************************************************************
 * function:数据解码失败是，会重新解析缓冲区中的数据，防止缓冲区中有数据夹帧的可能
 * retval:无
**********************************************************************************************************/
void SlaveProtocol::redecode()
{
    unsigned short hLen;         //记录待处理数据长度
    int iHeaderPos = -1;
    unsigned char cCheckSum = 0;
    unsigned short hFindIndex = 0;

//    qDebug()<<"redecode data : "<<Convert2Hex::array2hex(aBuffer, hBufDataLen)<<Qt::endl;

    hLen = hBufDataLen;
    hPos = 0;
    hFrameLen = 0;
    do
    {
        iHeaderPos = find_header(&aBuffer[hFindIndex], hLen - hFindIndex);        //重新寻找头码，防止还有短帧数据在缓冲区
//        qDebug()<<"find data : "<<Convert2Hex::array2hex(&aBuffer[hFindIndex], hLen - hFindIndex)<<Qt::endl;
//        qDebug()<<"redecode header pos : "<<iHeaderPos<<Qt::endl;
        if(iHeaderPos != -1)                    //找到了头码
        {
            if(hFindIndex + iHeaderPos + 1 == hLen)             //头码位于最后一位数据
            {
//                if(decode_fail_callback)                    //该丢弃的数据先做接收解码失败的响应
//                {
//                    decode_fail_callback(aBuffer, hFindIndex + iHeaderPos);
//                }
                hPos = 1;
                return;
            }
            hFrameLen = aBuffer[hFindIndex + iHeaderPos + 1];            //寻找新一帧帧尾

            if(hFrameLen < 2)
            {
                hFrameLen = 0;
                hPos = 0;
                hFindIndex++;
            }
            else if((hFrameLen + 2) > (hLen - hFindIndex - iHeaderPos))                      //剩余数据全部分析完毕，等待下次组帧
            {
//                qDebug()<<"will return : "<<Convert2Hex::array2hex(aBuffer, hBufDataLen)<<Qt::endl;
                if((decode_fail_callback != nullptr) && ((hFindIndex + iHeaderPos) > 0))     //该丢弃的数据先做接收解码失败的响应
                {
                    decode_fail_callback(aBuffer, hFindIndex + iHeaderPos);
                }
                memcpy(aBuffer, &aBuffer[hFindIndex + iHeaderPos], hLen - hFindIndex - iHeaderPos);
                hPos = hLen - hFindIndex - iHeaderPos;
                hBufDataLen = hPos;
                return;
            }
            else                                            //可能存在新一帧数据，需要校验解码
            {
                cCheckSum = check_sum(&aBuffer[hFindIndex + iHeaderPos], hFrameLen + 1);
                // std::cout<<"check sum : "<<array2hex(&cCheckSum, 1)<<std::endl;
                if(cCheckSum == aBuffer[hFindIndex + iHeaderPos + hFrameLen + 1])     //垃圾堆里找到一帧可用数据
                {
                    if(((hFindIndex + iHeaderPos) != 0) && (decode_fail_callback != nullptr))      //把失败的数据先响应完
                    {
//                        qDebug()<<"redecode fail, hFindIndex: "<<hFindIndex<<Qt::endl;
                        decode_fail_callback(aBuffer, hFindIndex + iHeaderPos);
                    }

                    if(decode_ok_callback)                              //再响应接收成功的数据
                    {
                        decode_ok_callback(&aBuffer[hFindIndex + iHeaderPos+ 2], hFrameLen - 1);         //响应新一帧数据
                    }

                    hLen = hLen - hFindIndex - iHeaderPos - hFrameLen - 2;                        //新一帧数据响应完毕，重新定义数据长度，并抛弃已经处理完毕的数据
                    memcpy(aBuffer, &aBuffer[hFindIndex + iHeaderPos + hFrameLen + 2], hLen);
                    hBufDataLen = hLen;
                    hPos = 0;
                    hFrameLen = 0;
                    hFindIndex = 0;
//                    qDebug()<<"remain data : "<<Convert2Hex::array2hex(aBuffer, hBufDataLen)<<Qt::endl;
                }
                else                        //数据校验有误，重新寻找下一帧
                {
                    hPos = 0;
                    hFrameLen = 0;
                    hFindIndex++;
                    if(hFindIndex >= hLen)          //特殊情况，数据全是A5，数据有误
                    {
                        if(decode_fail_callback)
                        {
//                            qDebug()<<"redecode fail 1, hFindIndex: "<<hFindIndex<<Qt::endl;
                            decode_fail_callback(aBuffer, hFindIndex);
                        }
                        return;
                    }
                }
            }
        }
        else
        {
            if(decode_fail_callback)
            {
//                qDebug()<<"redecode fail 2, hFindIndex: "<<hFindIndex<<Qt::endl;
                decode_fail_callback(aBuffer, hLen);
            }
            memset(aBuffer, 0, sizeof(hBufDataLen));
            hLen = 0;
            hBufDataLen = 0;
        }
    }while(hLen > 0);
}

/**********************************************************************************************************
 * function:用于解析接收到的数据
 * params:数据指针
 * params:数据长度
 * retval:0-解码成功，1-解码失败
 * slaveprotocol.cp
**********************************************************************************************************/
int SlaveProtocol::decode(unsigned char *pData, unsigned short len)
{
    RxTimer.stop();
    memcpy(&aBuffer[hBufDataLen], pData, len);
    // std::cout<<"rx data : "<<SlaveProtocol::array2hex(pData, len)<<std::endl;
    hBufDataLen += len;
    redecode();

    int res = 0;


    if(hPos != 0)
    {
        RxTimer.start();
    }
    else
    {
        RxTimer.stop();
    }

    // if (RxTimer.isRunning()) 
    // {  
    //     std::cout<<"rx timer is start"<<std::endl;
    // } else {  
    //     std::cout<<"rx timer is stop"<<std::endl;
    // }
    return res;
}

short hextodec(short x1, int id)
{
    short firstbit =0;
    firstbit = (x1 & 0x8000) >> 15;
        if (firstbit == 0) 
        {  
        // ROS_INFO("The byte is positive or zero." );
        // std::cout<<"decode ok x "<<id<<" = " << x1 <<std::endl;
        // ROS_INFO("Positive: data %d, Number: %d", id ,x1);
        return x1;
        } 
        else if (firstbit == 1){  
        // ROS_WARN("The byte is negative." );
        // ROS_INFO("Negative: x %d, Number: %d", id ,x1);
        // std::cout<<"decode ok x "<<id<<" = " <<std::dec<< x1 <<std::endl;
        return x1;
        }
}


// void decode_ok(unsigned char *pData, unsigned short len)
// {
//     std::string hdlData;
//     hdlData = SlaveProtocol::array2hex(pData, len);
// // hdlData = Convert2Hex::array2hex(pData, len);
// // //    qDebug()<<"decode ok : "<<hdlData<<Qt::endl;
//     std::cout<<"decode ok : "<<hdlData<<std::endl;
// }

// void decode_fail(unsigned char *pData, unsigned short len)
// {
// //     QString hdlData;
// std::string hdlData;
//     hdlData = SlaveProtocol::array2hex(pData, len);
// // //    qDebug()<<"decode fail : "<<hdlData<<Qt::endl;
// //     qDebug()<<"decode fail : "<<hdlData<<endl;
// std::cout<<"decode fail : "<< hdlData<<std::endl;
// }

// unsigned char aRxData[] = {0xA5, 0xD, 0x1, 0xA5, 0x8, 0x2, 0, 0x8, 0, 0x2, 0xFF, 0x3, 0xBB, 0xA5, 0x0, 0x1, 0xA5};
// unsigned char aRxData1[] = {0x5, 0xD, 0x1, 0xA5, 0x8, 0x65, 0, 0x8, 0, 0x2, 0xFF, 0x3, 0xBB, 0xA5, 0xA5, 0xA5, 0xA5};
// unsigned char aRxData2[] = {0x5, 0xD, 0x1, 0xA5, 0x8, 0x65};



// int main(int argc, char** argv)
// {
//     ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);
//     // ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Warn);
//     ros::init(argc, argv, "decode");
//     ros::NodeHandle hn;
//     //创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
//     SlaveProtocol *slave;
//     slave = new SlaveProtocol(hn, decode_ok, decode_fail);
    

//     ROS_INFO("length of aRxData: %d",(int)sizeof(aRxData));
//     slave->decode(aRxData, sizeof(aRxData));

//     ros::Duration(0.1).sleep();  
//     slave->decode(aRxData1, sizeof(aRxData1));
//     // int length = sizeof(slave->aBuffer) / sizeof(slave->aBuffer[0]);
//     // ROS_INFO("length of aBuffer: %d", length);
//     // for (int i = 0; i < length; i++) 
//     // {  
//     //     ROS_INFO("%02X ", slave->aBuffer[i]);  // 使用%02X格式化输出，以十六进制显示数组元素  
//     // }  
//     ros::spin();


//     return 0;
// }
