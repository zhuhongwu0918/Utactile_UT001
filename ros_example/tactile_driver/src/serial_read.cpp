//tactile_driver.cpp
#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>
#include  "tactile_driver/slaveprotocol.h"
#include <tactile_driver/contactpoint.h>  

// void serialData::decode_ok(unsigned char *pData, unsigned short len)
// {
//     sd->pubSensorInfo=n.advertise<tactile_driver::contactpoint>("/sensorInfo",100);//这个将作为话题名

// }
// std_msgs::String output;
tactile_driver::contactpoint tactilepoint;
// output.data = "global";
SlaveProtocol *slavepl;
void decode_ok(unsigned char *pData, unsigned short len)
{
    // std::cout<<"decode ok pData[0] : "<<pData[0]<<std::endl;
    short x1, y1, z1, x2, y2, z2, x3, y3, z3,x4, y4, z4,x5, y5, z5,x6, y6, z6,x7, y7, z7,x8, y8, z8;
    short x9, y9, z9,x10, y10, z10,x11, y11, z11,x12, y12, z12,x13, y13, z13,x14, y14, z14,x15, y15, z15, x16, y16, z16;
    // tactile_driver::contactpoint point;
    // point.point1x = 1;

    // output.data = "from decode_ok";
    // slavepl->pub_callback.publish(output);  ////zhhw
    // ROS_INFO("Enter decode_ok");
    // pubSensorInfo.publish(point);
    switch(pData[0])
    {
        case 0:

        x1 = ((short)pData[1] << 8) | pData[2];
        x1 = hextodec(x1, 1);
        y1 = ((short)pData[3] << 8) | pData[4];
        y1 = hextodec(y1, 1);
        z1 = ((short)pData[5] << 8) | pData[6];
        z1 = hextodec(z1, 1);

        x2 = ((short)pData[7] << 8) | pData[8];
        x2 = hextodec(x2, 2);
        y2 = ((short)pData[9] << 8) | pData[10];
        y2 = hextodec(y2, 2);
        z2 = ((short)pData[11] << 8) | pData[12];
        z2 = hextodec(z2, 2);

        x3 = ((short)pData[13] << 8) | pData[14];
        x3 = hextodec(x3, 3);
        y3 = ((short)pData[15] << 8) | pData[16];
        y3 = hextodec(y3, 3);
        z3 = ((short)pData[17] << 8) | pData[18];
        z3 = hextodec(z3, 3);

        x4 = ((short)pData[19] << 8) | pData[20];
        x4 = hextodec(x4, 4);
        y4 = ((short)pData[21] << 8) | pData[22];
        y4 = hextodec(y4, 4);
        z4 = ((short)pData[23] << 8) | pData[24];
        z4 = hextodec(z4, 4);

        x5 = ((short)pData[25] << 8) | pData[26];
        x5 = hextodec(x5, 5);
        y5 = ((short)pData[27] << 8) | pData[28];
        y5 = hextodec(y5, 5);
        z5 = ((short)pData[29] << 8) | pData[30];
        z5 = hextodec(z5, 5);
        
        x6 = ((short)pData[31] << 8) | pData[32];
        x6 = hextodec(x6, 6);
        y6 = ((short)pData[33] << 8) | pData[34];
        y6 = hextodec(y6, 6);
        z6 = ((short)pData[35] << 8) | pData[36];
        z6 = hextodec(z6, 6);
        
        x7 = ((short)pData[37] << 8) | pData[38];
        x7 = hextodec(x7, 7);
        y7 = ((short)pData[39] << 8) | pData[40];
        y7 = hextodec(y7, 7);
        z7 = ((short)pData[41] << 8) | pData[42];
        z7 = hextodec(z7, 7);

        x8 = ((short)pData[43] << 8) | pData[44];
        x8 = hextodec(x8, 8);
        y8 = ((short)pData[45] << 8) | pData[46];
        y8 = hextodec(y8, 8);
        z8 = ((short)pData[47] << 8) | pData[48];
        z8 = hextodec(z8, 8);
        
        x9 = ((short)pData[49] << 8) | pData[50];
        x9 = hextodec(x9, 9);
        y9 = ((short)pData[51] << 8) | pData[52];
        y9 = hextodec(y9, 9);
        z9 = ((short)pData[53] << 8) | pData[54];
        z9 = hextodec(z9, 9);

        x10 = ((short)pData[55] << 8) | pData[56];
        x10 = hextodec(x10, 10);
        y10 = ((short)pData[57] << 8) | pData[58];
        y10 = hextodec(y10, 10);
        z10 = ((short)pData[59] << 8) | pData[60];
        z10 = hextodec(z10, 10);

        x11 = ((short)pData[61] << 8) | pData[62];
        x11 = hextodec(x11, 10);
        y11 = ((short)pData[63] << 8) | pData[64];
        y11 = hextodec(y11, 10);
        z11 = ((short)pData[65] << 8) | pData[66];
        z11 = hextodec(z11, 10);

        x12 = ((short)pData[67] << 8) | pData[68];
        x12 = hextodec(x12, 10);
        y12 = ((short)pData[69] << 8) | pData[70];
        y12 = hextodec(y12, 10);
        z12 = ((short)pData[71] << 8) | pData[72];
        z12 = hextodec(z12, 10);    

        x13 = ((short)pData[73] << 8) | pData[74];
        x13 = hextodec(x13, 10);
        y13 = ((short)pData[75] << 8) | pData[76];
        y13 = hextodec(y13, 10);
        z13 = ((short)pData[77] << 8) | pData[78];
        z13 = hextodec(z13, 10);     

        x14 = ((short)pData[79] << 8) | pData[80];
        x14 = hextodec(x14, 10);
        y14 = ((short)pData[81] << 8) | pData[82];
        y14 = hextodec(y14, 10);
        z14 = ((short)pData[83] << 8) | pData[84];
        z14 = hextodec(z14, 10);  

        x15 = ((short)pData[85] << 8) | pData[86];
        x15 = hextodec(x15, 10);
        y15 = ((short)pData[87] << 8) | pData[88];
        y15 = hextodec(y15, 10);
        z15 = ((short)pData[89] << 8) | pData[90];
        z15 = hextodec(z15, 10);  

        x16 = ((short)pData[91] << 8) | pData[92];
        x16 = hextodec(x16, 10);
        y16 = ((short)pData[93] << 8) | pData[94];
        y16 = hextodec(y16, 10);
        z16 = ((short)pData[95] << 8) | pData[96];
        z16 = hextodec(z16, 10);  
        tactilepoint.point1x = x1;
        tactilepoint.point1y = y1;
        tactilepoint.point1z = z1;
        tactilepoint.point2x = x2;
        tactilepoint.point2y = y2;
        tactilepoint.point2z = z2;
        tactilepoint.point3x = x3;
        tactilepoint.point3y = y3;
        tactilepoint.point3z = z3;
        tactilepoint.point4x = x4;
        tactilepoint.point4y = y4;
        tactilepoint.point4z = z4;

        tactilepoint.point5x = x5;
        tactilepoint.point5y = y5;
        tactilepoint.point5z = z5;
        tactilepoint.point6x = x6;
        tactilepoint.point6y = y6;
        tactilepoint.point6z = z6;
        tactilepoint.point7x = x7;
        tactilepoint.point7y = y7;
        tactilepoint.point7z = z7;
        tactilepoint.point8x = x8;
        tactilepoint.point8y = y8;
        tactilepoint.point8z = z8;

        tactilepoint.point9x = x9;
        tactilepoint.point9y = y9;
        tactilepoint.point9z = z9;
        tactilepoint.point10x = x10;
        tactilepoint.point10y = y10;
        tactilepoint.point10z = z10;
        tactilepoint.point11x = x11;
        tactilepoint.point11y = y11;
        tactilepoint.point11z = z11;
        tactilepoint.point12x = x12;
        tactilepoint.point12y = y12;
        tactilepoint.point12z = z12;

        tactilepoint.point13x = x13;
        tactilepoint.point13y = y13;
        tactilepoint.point13z = z13;
        tactilepoint.point14x = x14;
        tactilepoint.point14y = y14;
        tactilepoint.point14z = z14;
        tactilepoint.point15x = x15;
        tactilepoint.point15y = y15;
        tactilepoint.point15z = z15;
        tactilepoint.point16x = x16;
        tactilepoint.point16y = y16;
        tactilepoint.point16z = z16;
        
        slavepl->pub_tactile.publish(tactilepoint);  ////zhhw
        // ROS_INFO("Enter decode_ok: %d", tactilepoint.point1x);

        break;
        case 1:
        break;
    }
    std::string hdlData;
    hdlData = SlaveProtocol::array2hex(pData, len);
}

void decode_fail(unsigned char *pData, unsigned short len)
{
//     QString hdlData;
    unsigned char value = SlaveProtocol::check_sum(pData, len - 1);
std::string hdlData;
    hdlData = SlaveProtocol::array2hex(pData, len);
// //    qDebug()<<"decode fail : "<<hdlData<<Qt::endl;
//     qDebug()<<"decode fail : "<<hdlData<<endl;

// std::cout<<"decode fail : "<< hdlData<<std::endl;
// std::cout<<"decode fail check sum: "<< SlaveProtocol::array2hex(&value, 1)<<std::endl;
}




int main(int argc, char** argv)
{
    ros::init(argc, argv, "tactile_driver");
//创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
    ros::NodeHandle n;
    // SlaveProtocol *slavepl = new SlaveProtocol(n, decode_ok, decode_fail);
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);
    // ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Warn);

    slavepl = new SlaveProtocol(n, decode_ok, decode_fail);

   //创建一个serial类
    serial::Serial sp;
    //创建timeout
    serial::Timeout to = serial::Timeout::simpleTimeout(100);
    //设置要打开的串口名称
    sp.setPort("/dev/ttyUSB0");
    //设置串口通信的波特率
    sp.setBaudrate(115200);
    //串口设置timeout
    sp.setTimeout(to);
 
    try
    {
        //打开串口
        sp.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port.");
        return -1;
    }
    
    //判断串口是否打开成功
    if(sp.isOpen())
    {
        ROS_INFO_STREAM("/dev/ttyUSB0 is opened.");
    }
    else
    {
        return -1;
    }
    
    ros::Rate loop_rate(500);
    while(ros::ok())
    {
        //获取缓冲区内的字节数
        size_t n = sp.available();
        if(n!=0)
        {
            uint8_t buffer[1024];
            //读出数据
            n = sp.read(buffer, n);
            int k=0;
            // for(int i=0; i<n; i++)
            // {
            //     // 16进制的方式打印到屏幕
            //     ROS_INFO("raw in buffer:");
            //     std::cout << std::hex << (buffer[i] & 0xff) << " ";

            //     if(buffer[i] == 0xa5)
            //     {
            //         int raw[k] = buffer[i];
            //         k++;
            //     }
            // }
            // std::cout <<"******buffer end"<< std::endl;
            //把数据发送回去
            // sp.write(buffer, n);
            slavepl->decode(buffer, n);

            // slavepl->pub_callback.publish(output);
            // slavepl->pub_tactile.publish(tactilepoint);

        }


        loop_rate.sleep();
    }
    
    //关闭串口
    sp.close();
 
    return 0;
}
