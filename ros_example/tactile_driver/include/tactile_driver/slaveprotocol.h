//slaveprotocol.h
#ifndef SLAVEPROTOCOL_H
#define SLAVEPROTOCOL_H
#include<ros/ros.h>
// #include <QTimer>

using namespace std;

#define RX_BUFFER_SIZE      500
short hextodec(short x1, int id);

enum
{
    AXIS_ARRAY_DATA = 0,            //阵列数据
};

class SlaveProtocol
{
public:
    SlaveProtocol(ros::NodeHandle &node);
    SlaveProtocol(ros::NodeHandle &node, void (*decode_ok)(unsigned char *pData, unsigned short len), void (*decode_fail)(unsigned char *pData, unsigned short len));

    // int decode(unsigned char *pData, unsigned short len);
    int decode(unsigned char *pData, unsigned short len);
    static std::string array2hex(unsigned char *pData, unsigned short len);
    static char hex2char(unsigned char value);
    void rx_timerout_callback(const ros::TimerEvent&);
    static unsigned char check_sum(unsigned char *pData, unsigned short len);          //校验数据

    unsigned char aBuffer[RX_BUFFER_SIZE]; 
///////zhhw
    public:  
    //   ros::Publisher pub_;  
    //   ros::Publisher pub_callback; 
      ros::Publisher  pub_tactile;
      ros::Subscriber sub_;  

private:
             //用来存储帧数据的buffer
    unsigned short hPos = 0;            //用来指示当前完整帧的位置，为0时表示当前没有识别到帧数据
    unsigned short hFrameLen = 0;             //记录当前帧的帧长度
    // QTimer *RxTimer;
    ros::Timer RxTimer;
    ros::NodeHandle nh_;  

    unsigned short hBufDataLen = 0;

    void (*decode_ok_callback)(unsigned char *pData, unsigned short len);      //当一帧数据被正确解析出来后，会调用该函数
    void (*decode_fail_callback)(unsigned char *pData, unsigned short len);    //当一帧数据解析出错时，会调用该函数
    
    int find_header(unsigned char *pData, unsigned short len);                 //寻找数据头
    void redecode();
    /////////////////////////

};

#endif // SLAVEPROTOCOL_H