#include "ros/ros.h"
#include <std_msgs/Float32MultiArray.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// create an ofstream for the file output (see the link on streams for
// more info)
fstream fout;
// create a name for the file output
std::string filename = "test.csv";


void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg)
{
    ROS_INFO("pitch: [%f], roll: [%f], pos_x: [%f], pos_y: [%f], ori_z: [%f], ori_w: [%f]", msg->data[0], msg->data[1], msg->data[2], 
        msg->data[3], msg->data[4], msg->data[5]); //not sure

    fout << msg->data[0] << "," << msg->data[1] << "," << msg->data[2] << "," << msg->data[3]<< "," 
        << msg->data[4] << "," << msg->data[5] << "\n";

}

int main(int argc, char **argv)
{

    fout.open(filename.c_str(), ios::out | ios::app);
    fout << "Pitch" << "," << "Roll" << "pos_x" << "," << "pos_y" << "ori_z" << "," << "ori_w" << "\n";
    
    ros::init(argc, argv, "listener");
    ros::NodeHandle n;
    ros::Subscriber sub1 = n.subscribe("data_pub", 1000, chatterCallback);
    //ros::Subscriber sub2 = n.subscribe("roll_pub", 1000, chatterCallback);

    ros::spin();
    
    fout.close();
    return 0;
}

