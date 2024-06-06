mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
catkin_init_workspace

cd ~/catkin_ws/
catkin_make
#if in conda env,use this command instead
catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3

#copy tactile_driver directory into src floder

#instal rosserial package
sudo apt-get install ros-noetic-serial  #ros为Kinect版本
sudo apt-get install ros-melodic-serial  #ros为melodic版本

cd ~/catkin_ws && catkin_make
source devel/setup.bash

#read serial
ls /dev/ttyACM0
OR
ls /dev/ttyUSB0

sudo chmod 777 /dev/ttyACM0
OR
sudo chmod 777 /dev/ttyUSB0

#launch with UI
roslaunch tactile_driver demo.launch
