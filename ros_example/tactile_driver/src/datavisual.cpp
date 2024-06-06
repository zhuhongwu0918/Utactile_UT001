#include <ros/ros.h>  
#include"std_msgs/String.h"
#include "visualization_msgs/Marker.h"
#include <eigen3/Eigen/Eigen>
#include "tactile_driver/contactpoint.h"
//#include "tactile_driver/contactpoint.h"
Eigen::Vector3d vector2matrix2eulerAngle(Eigen::Vector3d tmpvec){
         Eigen::Vector3d zaxis(0, 0, 1);
        //   Eigen::Vector3d tmpvec(vx, vy, vz);
          tmpvec.normalize();
 
          Eigen::Vector3d xaxis = zaxis.cross(tmpvec);
          xaxis.normalize();
          Eigen::Vector3d yaxis = tmpvec.cross(xaxis);
          yaxis.normalize();

        Eigen::Matrix3d m;
          m(0, 0) = xaxis(0);
          m(0, 1) = yaxis(0);
          m(0, 2) = tmpvec(0);
 
          m(1, 0) = xaxis(1);
          m(1, 1) = yaxis(1);
          m(1, 2) = tmpvec(1);
 
          m(2, 0) = xaxis(2);
          m(2, 1) = yaxis(2);
          m(2, 2) = tmpvec(2);

          Eigen::Vector3d eulerAngle = m.eulerAngles(0,1,2);
          return eulerAngle;
}


class SubscribeAndPublish  
{  
    public:  

    SubscribeAndPublish()  
    {  
    //Topic you want to publish  
        pub_marker_1 = nh.advertise<visualization_msgs::Marker>("visualization_marker1", 10);//initialize marker publisher
        pub_marker_2 = nh.advertise<visualization_msgs::Marker>("visualization_marker2", 10);//initialize marker publisher
        pub_marker_3 = nh.advertise<visualization_msgs::Marker>("visualization_marker3", 10);//initialize marker publisher
        pub_marker_4 = nh.advertise<visualization_msgs::Marker>("visualization_marker4", 10);//initialize marker publisher

        pub_marker_5 = nh.advertise<visualization_msgs::Marker>("visualization_marker5", 10);//initialize marker publisher
        pub_marker_6 = nh.advertise<visualization_msgs::Marker>("visualization_marker6", 10);//initialize marker publisher
        pub_marker_7 = nh.advertise<visualization_msgs::Marker>("visualization_marker7", 10);//initialize marker publisher
        pub_marker_8 = nh.advertise<visualization_msgs::Marker>("visualization_marker8", 10);//initialize marker publisher

        pub_marker_9 = nh.advertise<visualization_msgs::Marker>("visualization_marker9", 10);//initialize marker publisher
        pub_marker_10 = nh.advertise<visualization_msgs::Marker>("visualization_marker10", 10);//initialize marker publisher
        pub_marker_11 = nh.advertise<visualization_msgs::Marker>("visualization_marker11", 10);//initialize marker publisher
        pub_marker_12 = nh.advertise<visualization_msgs::Marker>("visualization_marker12", 10);//initialize marker publisher

        pub_marker_13 = nh.advertise<visualization_msgs::Marker>("visualization_marker13", 10);//initialize marker publisher
        pub_marker_14 = nh.advertise<visualization_msgs::Marker>("visualization_marker14", 10);//initialize marker publisher
        pub_marker_15 = nh.advertise<visualization_msgs::Marker>("visualization_marker15", 10);//initialize marker publisher
        pub_marker_16 = nh.advertise<visualization_msgs::Marker>("visualization_marker16", 10);//initialize marker publisher
        //Topic you want to subscribe  
        submarker = nh.subscribe("/tactile_data", 10, &SubscribeAndPublish::callback, this);  //注意这里，和平时使用回调函数不一样了。
    }  
        

    void callback(const tactile_driver::contactpoint& input)  
    {  
        tactile_driver::contactpoint output;  
        output = input;
        int16_t ca=3.5;
        //.... do something with the input and generate the output...
        ////////////////////// 1-4///////////////////////////
        Eigen::Vector3d tmpvec(output.point1x,output.point1y,output.point1z);
        Eigen::Vector3d eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        Eigen::Quaterniond q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_1.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_1.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_1.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_1.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_1.scale.x = (((sqrt(pow(output.point1x,2)+pow(output.point1y,2)+pow(output.point1z,2))))/32767)*ca;

        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_1, SubscribeAndPublish::pub_marker_1);

        tmpvec = Eigen::Vector3d(output.point2x,output.point2y,output.point2z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_2.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_2.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_2.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_2.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_2.scale.x = (((sqrt(pow(output.point2x,2)+pow(output.point2y,2)+pow(output.point2z,2))))/32767)*ca;

        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_2, SubscribeAndPublish::pub_marker_2);

        tmpvec = Eigen::Vector3d(output.point3x,output.point3y,output.point3z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_3.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_3.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_3.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_3.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_3.scale.x = (((sqrt(pow(output.point3x,2)+pow(output.point3y,2)+pow(output.point3z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_3, SubscribeAndPublish::pub_marker_3);

        tmpvec = Eigen::Vector3d(output.point4x,output.point4y,output.point4z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_4.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_4.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_4.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_4.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_4.scale.x = (((sqrt(pow(output.point4x,2)+pow(output.point4y,2)+pow(output.point4z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_4, SubscribeAndPublish::pub_marker_4);
        /////////////////5-8////////////////////////////////////////////////////////////////////////////////////
        tmpvec = Eigen::Vector3d(output.point5x,output.point5y,output.point5z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_5.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_5.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_5.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_5.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_5.scale.x = (((sqrt(pow(output.point5x,2)+pow(output.point5y,2)+pow(output.point5z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_5, SubscribeAndPublish::pub_marker_5);

        tmpvec = Eigen::Vector3d(output.point6x,output.point6y,output.point6z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_6.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_6.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_6.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_6.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_6.scale.x = (((sqrt(pow(output.point6x,2)+pow(output.point6y,2)+pow(output.point6z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_6, SubscribeAndPublish::pub_marker_6);

        tmpvec = Eigen::Vector3d(output.point7x,output.point7y,output.point7z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_7.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_7.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_7.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_7.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_7.scale.x = (((sqrt(pow(output.point7x,2)+pow(output.point7y,2)+pow(output.point7z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_7, SubscribeAndPublish::pub_marker_7);

        tmpvec = Eigen::Vector3d(output.point8x,output.point8y,output.point8z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_8.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_8.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_8.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_8.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_8.scale.x = (((sqrt(pow(output.point8x,2)+pow(output.point8y,2)+pow(output.point8z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_8, SubscribeAndPublish::pub_marker_8);
        //////////////9-12///////////////////////////////////////////////////////////////////////////////////////
        tmpvec = Eigen::Vector3d(output.point9x,output.point9y,output.point9z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_9.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_9.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_9.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_9.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_9.scale.x = (((sqrt(pow(output.point9x,2)+pow(output.point9y,2)+pow(output.point9z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_9, SubscribeAndPublish::pub_marker_9);

        tmpvec = Eigen::Vector3d(output.point10x,output.point10y,output.point10z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_10.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_10.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_10.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_10.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_10.scale.x = (((sqrt(pow(output.point10x,2)+pow(output.point10y,2)+pow(output.point10z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_10, SubscribeAndPublish::pub_marker_10);

        tmpvec = Eigen::Vector3d(output.point11x,output.point11y,output.point11z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_11.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_11.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_11.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_11.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_11.scale.x = (((sqrt(pow(output.point11x,2)+pow(output.point11y,2)+pow(output.point11z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_11, SubscribeAndPublish::pub_marker_11);

        tmpvec = Eigen::Vector3d(output.point12x,output.point12y,output.point12z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_12.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_12.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_12.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_12.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_12.scale.x = (((sqrt(pow(output.point12x,2)+pow(output.point12y,2)+pow(output.point12z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_12, SubscribeAndPublish::pub_marker_12);

        //////////////13-16///////////////////////////////////////////////////////////////////////////////////////
        tmpvec = Eigen::Vector3d(output.point13x,output.point13y,output.point13z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_13.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_13.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_13.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_13.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_13.scale.x = (((sqrt(pow(output.point13x,2)+pow(output.point13y,2)+pow(output.point13z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_13, SubscribeAndPublish::pub_marker_13);

        tmpvec = Eigen::Vector3d(output.point14x,output.point14y,output.point14z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_14.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_14.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_14.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_14.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_14.scale.x = (((sqrt(pow(output.point14x,2)+pow(output.point14y,2)+pow(output.point14z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_14, SubscribeAndPublish::pub_marker_14);

        tmpvec = Eigen::Vector3d(output.point15x,output.point15y,output.point15z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_15.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_15.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_15.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_15.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_15.scale.x = (((sqrt(pow(output.point15x,2)+pow(output.point15y,2)+pow(output.point15z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_15, SubscribeAndPublish::pub_marker_15);

        tmpvec = Eigen::Vector3d(output.point16x,output.point16y,output.point16z);
        eulerAngle = vector2matrix2eulerAngle(tmpvec);
        eulerAngle.z() = eulerAngle.z() - 1.57;
        q = SubscribeAndPublish::euler2Quaternion(eulerAngle.x(), eulerAngle.y(), eulerAngle.z());
        SubscribeAndPublish::marker_16.pose.orientation.x = q.x();
        SubscribeAndPublish::marker_16.pose.orientation.y = q.y();
        SubscribeAndPublish::marker_16.pose.orientation.z = q.z();
        SubscribeAndPublish::marker_16.pose.orientation.w = q.w();

        SubscribeAndPublish::marker_16.scale.x = (((sqrt(pow(output.point16x,2)+pow(output.point16y,2)+pow(output.point16z,2))))/32767)*ca;
        SubscribeAndPublish::Publish(SubscribeAndPublish::marker_16, SubscribeAndPublish::pub_marker_16);

        // ROS_INFO("callbacked %d", output.point1x);
        // ROS_INFO("callback published");
    }    

    void Publish(visualization_msgs::Marker &marker_ , ros::Publisher &pub_marker)
    {
        marker_ = marker_;
        // while (pub_marker_.getNumSubscribers() < 1) {
        //     sleep(1);
        // }
        marker_.header.stamp = ros::Time();
        pub_marker.publish(marker_);
    };

    void set_marker_fixed_property(visualization_msgs::Marker &marker_)
    {
            marker_.header.frame_id = "/base_link";
            marker_.ns = "shapes";
            marker_.id = 0;
            
            marker_.type = visualization_msgs::Marker::ARROW;
            
            marker_.action = visualization_msgs::Marker::ADD;
            marker_.lifetime = ros::Duration(); //(sec,nsec),0 forever
            marker_.pose.position.x = 0.5;
            marker_.pose.position.y = 1.1;
            marker_.pose.position.z = 0.4;

            marker_.pose.orientation.x = 1;
            marker_.pose.orientation.y = 1;
            marker_.pose.orientation.z = 0;
            marker_.pose.orientation.w = 0.;
            
            marker_.scale.x = 0.05;
            marker_.scale.y = 0.05;
            marker_.scale.z = 0.05;
            
            marker_.color.r = 1.;
            marker_.color.g = 0.3;
            marker_.color.b = 0.;
            marker_.color.a = 1.0;
    };

    Eigen::Quaterniond euler2Quaternion(const double roll, const double pitch, const double yaw)  
    {  
        Eigen::AngleAxisd rollAngle(roll, Eigen::Vector3d::UnitZ());  
        Eigen::AngleAxisd yawAngle(yaw, Eigen::Vector3d::UnitY());  
        Eigen::AngleAxisd pitchAngle(pitch, Eigen::Vector3d::UnitX());  

        Eigen::Quaterniond q = rollAngle * yawAngle * pitchAngle; 
        return q;  
    } 

        
    public:  
        ros::NodeHandle nh;   
        ros::Publisher pub_callback; 
        ros::Subscriber submarker;  
        ros::Publisher  pub_marker_1 , pub_marker_2,pub_marker_3 , pub_marker_4;
        ros::Publisher  pub_marker_5 , pub_marker_6,pub_marker_7, pub_marker_8;
        ros::Publisher  pub_marker_9 , pub_marker_10,pub_marker_11, pub_marker_12;
        ros::Publisher  pub_marker_13 , pub_marker_14,pub_marker_15 , pub_marker_16;

        visualization_msgs::Marker marker_1, marker_2, marker_3, marker_4;
        visualization_msgs::Marker marker_5, marker_6, marker_7, marker_8;
        visualization_msgs::Marker marker_9, marker_10, marker_11, marker_12;
        visualization_msgs::Marker marker_13, marker_14, marker_15, marker_16;

};//End of class SubscribeAndPublish  
    
int main(int argc, char **argv)  
{  
      //Initiate ROS  
      ros::init(argc, argv, "marker_worker");  
      
      //Create an object of class SubscribeAndPublish that will take care of everything  
      SubscribeAndPublish SAPObject;  
      std_msgs::String msg; 
      msg.data = "from main";
      ros::Rate loop_rate(10);
    float f = 0.0;
    float x = 0.0;
    float y = 0.0;
    SAPObject.set_marker_fixed_property(SAPObject.marker_1);
    SAPObject.set_marker_fixed_property(SAPObject.marker_2);
    SAPObject.set_marker_fixed_property(SAPObject.marker_3);
    SAPObject.set_marker_fixed_property(SAPObject.marker_4);

    SAPObject.set_marker_fixed_property(SAPObject.marker_5);
    SAPObject.set_marker_fixed_property(SAPObject.marker_6);
    SAPObject.set_marker_fixed_property(SAPObject.marker_7);
    SAPObject.set_marker_fixed_property(SAPObject.marker_8);

    SAPObject.set_marker_fixed_property(SAPObject.marker_9);
    SAPObject.set_marker_fixed_property(SAPObject.marker_10);
    SAPObject.set_marker_fixed_property(SAPObject.marker_11);
    SAPObject.set_marker_fixed_property(SAPObject.marker_12);

    SAPObject.set_marker_fixed_property(SAPObject.marker_13);
    SAPObject.set_marker_fixed_property(SAPObject.marker_14);
    SAPObject.set_marker_fixed_property(SAPObject.marker_15);
    SAPObject.set_marker_fixed_property(SAPObject.marker_16);


    while (ros::ok())
    {

        ros::spinOnce();//处理订阅话题的所有回调函数callback();

        float x = 0.1 * sin(50*f);
        //   ROS_INFO("x ,%f", x);
        float y = 0.1 * cos(30*f);

        double roll =0, pitch=0, yaw=- 3.14/2;
        Eigen::Quaterniond q = SAPObject.euler2Quaternion(roll, pitch, yaw);
        pitch = 0.05 * sin(50*f);
        // ROS_INFO("x ,%f", x);
        yaw = 0.05 * cos(100*f);
        q = SAPObject.euler2Quaternion(roll, pitch, yaw- 3.14/2);

        SAPObject.marker_1.pose.position.x = 0.5 + 0.8*0;
        SAPObject.marker_1.pose.position.y = 1.1;
        // SAPObject.marker_1.pose.orientation.x = q.x();
        // SAPObject.marker_1.pose.orientation.y = q.y();
        // SAPObject.marker_1.pose.orientation.z = q.z();
        // SAPObject.marker_1.pose.orientation.w = q.w();

        SAPObject.marker_2.pose.position.x = 0.5 + 0.8;
        SAPObject.marker_2.pose.position.y = 1.1;
        // SAPObject.marker_2.pose.orientation.x = q.x();
        // SAPObject.marker_2.pose.orientation.y = q.y();
        // SAPObject.marker_2.pose.orientation.z = q.z();
        // SAPObject.marker_2.pose.orientation.w = q.w();

        
        SAPObject.marker_3.pose.position.x = 0.5 + 0.8*2;
        SAPObject.marker_3.pose.position.y = 1.1;
        // SAPObject.marker_3.pose.orientation.x = q.x();
        // SAPObject.marker_3.pose.orientation.y = q.y();
        // SAPObject.marker_3.pose.orientation.z = q.z();
        // SAPObject.marker_3.pose.orientation.w = q.w();

        SAPObject.marker_4.pose.position.x = 0.5 + 0.8*3;
        SAPObject.marker_4.pose.position.y = 1.1 ;
        // SAPObject.marker_4.pose.orientation.x = q.x();
        // SAPObject.marker_4.pose.orientation.y = q.y();
        // SAPObject.marker_4.pose.orientation.z = q.z();
        // SAPObject.marker_4.pose.orientation.w = q.w();

        SAPObject.marker_5.pose.position.x = 0.5 + 0.8*0;
        SAPObject.marker_5.pose.position.y = 1.1 + 1.1*1;
        // SAPObject.marker_5.pose.orientation.x = q.x();
        // SAPObject.marker_5.pose.orientation.y = q.y();
        // SAPObject.marker_5.pose.orientation.z = q.z();
        // SAPObject.marker_5.pose.orientation.w = q.w();

        SAPObject.marker_6.pose.position.x = 0.5 + 0.8*1;
        SAPObject.marker_6.pose.position.y = 1.1 + 1.1*1;
        // SAPObject.marker_6.pose.orientation.x = q.x();
        // SAPObject.marker_6.pose.orientation.y = q.y();
        // SAPObject.marker_6.pose.orientation.z = q.z();
        // SAPObject.marker_6.pose.orientation.w = q.w();

        SAPObject.marker_7.pose.position.x = 0.5 + 0.8*2;
        SAPObject.marker_7.pose.position.y = 1.1+ 1.1*1;
        // SAPObject.marker_7.pose.orientation.x = q.x();
        // SAPObject.marker_7.pose.orientation.y = q.y();
        // SAPObject.marker_7.pose.orientation.z = q.z();
        // SAPObject.marker_7.pose.orientation.w = q.w();

        SAPObject.marker_8.pose.position.x = 0.5 + 0.8*3;
        SAPObject.marker_8.pose.position.y = 1.1 + 1.1*1;
        // SAPObject.marker_8.pose.orientation.x = q.x();
        // SAPObject.marker_8.pose.orientation.y = q.y();
        // SAPObject.marker_8.pose.orientation.z = q.z();
        // SAPObject.marker_8.pose.orientation.w = q.w();

        SAPObject.marker_9.pose.position.x = 0.5 + 0.8*0;
        SAPObject.marker_9.pose.position.y = 1.1 + 1.1*2;
        // SAPObject.marker_9.pose.orientation.x = q.x();
        // SAPObject.marker_9.pose.orientation.y = q.y();
        // SAPObject.marker_9.pose.orientation.z = q.z();
        // SAPObject.marker_9.pose.orientation.w = q.w();

        SAPObject.marker_10.pose.position.x = 0.5 + 0.8*1;
        SAPObject.marker_10.pose.position.y = 1.1 + 1.1*2;
        // SAPObject.marker_10.pose.orientation.x = q.x();
        // SAPObject.marker_10.pose.orientation.y = q.y();
        // SAPObject.marker_10.pose.orientation.z = q.z();
        // SAPObject.marker_10.pose.orientation.w = q.w();

        SAPObject.marker_11.pose.position.x = 0.5 + 0.8*2;
        SAPObject.marker_11.pose.position.y = 1.1 + 1.1*2;
        // SAPObject.marker_11.pose.orientation.x = q.x();
        // SAPObject.marker_11.pose.orientation.y = q.y();
        // SAPObject.marker_11.pose.orientation.z = q.z();
        // SAPObject.marker_11.pose.orientation.w = q.w();

        SAPObject.marker_12.pose.position.x = 0.5 + 0.8*3;
        SAPObject.marker_12.pose.position.y = 1.1 + 1.1*2;
        // SAPObject.marker_12.pose.orientation.x = q.x();
        // SAPObject.marker_12.pose.orientation.y = q.y();
        // SAPObject.marker_12.pose.orientation.z = q.z();
        // SAPObject.marker_12.pose.orientation.w = q.w();

        SAPObject.marker_13.pose.position.x = 0.5 + 0.8*0;
        SAPObject.marker_13.pose.position.y = 1.1 + 1.1*3;
        // SAPObject.marker_13.pose.orientation.x = q.x();
        // SAPObject.marker_13.pose.orientation.y = q.y();
        // SAPObject.marker_13.pose.orientation.z = q.z();
        // SAPObject.marker_13.pose.orientation.w = q.w();

        SAPObject.marker_14.pose.position.x = 0.5 + 0.8*1;
        SAPObject.marker_14.pose.position.y = 1.1 + 1.1*3;
        // SAPObject.marker_14.pose.orientation.x = q.x();
        // SAPObject.marker_14.pose.orientation.y = q.y();
        // SAPObject.marker_14.pose.orientation.z = q.z();
        // SAPObject.marker_14.pose.orientation.w = q.w();

        SAPObject.marker_15.pose.position.x = 0.5 + 0.8*2;
        SAPObject.marker_15.pose.position.y = 1.1 + 1.1*3;
        // SAPObject.marker_15.pose.orientation.x = q.x();
        // SAPObject.marker_15.pose.orientation.y = q.y();
        // SAPObject.marker_15.pose.orientation.z = q.z();
        // SAPObject.marker_15.pose.orientation.w = q.w();

        SAPObject.marker_16.pose.position.x = 0.5 + 0.8*3;
        SAPObject.marker_16.pose.position.y = 1.1 + 1.1*3;
        // SAPObject.marker_16.pose.orientation.x = q.x();
        // SAPObject.marker_16.pose.orientation.y = q.y();
        // SAPObject.marker_16.pose.orientation.z = q.z();
        // SAPObject.marker_16.pose.orientation.w = q.w();

        // SAPObject.Publish(SAPObject.marker_1, SAPObject.pub_marker_1);
        // SAPObject.Publish(SAPObject.marker_2, SAPObject.pub_marker_2);
        // SAPObject.Publish(SAPObject.marker_3, SAPObject.pub_marker_3);
        // SAPObject.Publish(SAPObject.marker_4, SAPObject.pub_marker_4);
        
        // SAPObject.Publish(SAPObject.marker_5, SAPObject.pub_marker_5);
        // SAPObject.Publish(SAPObject.marker_6, SAPObject.pub_marker_6);
        // SAPObject.Publish(SAPObject.marker_7, SAPObject.pub_marker_7);
        // SAPObject.Publish(SAPObject.marker_8, SAPObject.pub_marker_8);

        // SAPObject.Publish(SAPObject.marker_9, SAPObject.pub_marker_9);
        // SAPObject.Publish(SAPObject.marker_10, SAPObject.pub_marker_10);
        // SAPObject.Publish(SAPObject.marker_11, SAPObject.pub_marker_11);
        // SAPObject.Publish(SAPObject.marker_12, SAPObject.pub_marker_12);

        // SAPObject.Publish(SAPObject.marker_13, SAPObject.pub_marker_13);
        // SAPObject.Publish(SAPObject.marker_14, SAPObject.pub_marker_14);
        // SAPObject.Publish(SAPObject.marker_15, SAPObject.pub_marker_15);
        // SAPObject.Publish(SAPObject.marker_16, SAPObject.pub_marker_16);

        // ROS_INFO("main published on /published_topic");

        loop_rate.sleep();
        f += 0.04;
    }

return 0;  

};
