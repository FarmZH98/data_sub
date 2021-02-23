//v3 is subscribe to odom and publish command to arduino on when to start collecting data
//Then, it will string the odom pos and data together
#include "ros/ros.h"
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace std;

// create an ofstream for the file output (see the link on streams for
// more info)
fstream fout;
// create a name for the file output
std::string filename = "test.csv";
int collectionFlag=0;
float data_arr[4] = {0};


void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg)
{
    ROS_INFO("pitch: [%f], roll: [%f], pos_x: [%f], pos_y: [%f], ori_z: [%f], ori_w: [%f]", msg->data[0], msg->data[1], data_arr[0], 
        data_arr[1], data_arr[2], data_arr[3]); 

    fout << msg->data[0] << "," << msg->data[1] << "," << data_arr[0] << "," << data_arr[1]<< "," 
        << data_arr[2] << "," << data_arr[3] << "\n";

}

void odomCallback(const nav_msgs::Odometry::ConstPtr& OdomMsg)
{
    //ROS_INFO("Collecting OdomMsg");

    if(abs(OdomMsg->twist.twist.linear.x <=0.005) && abs(OdomMsg->twist.twist.linear.y <= 0.005) && abs(OdomMsg->twist.twist.angular.z <=0.05)){ //1. try to modify to roughly 0.05. 2. others that had the same prob: got if statement too. 3. delay
        collectionFlag=1;
        tf::Pose pose;
        tf::poseMsgToTF(OdomMsg->pose.pose, pose);
        double yaw_angle = tf::getYaw(pose.getRotation());
        data_arr[0]=OdomMsg->pose.pose.position.x; //still need modification here
        data_arr[1]=OdomMsg->pose.pose.position.y;
        data_arr[2]= yaw_angle; //OdomMsg->pose.pose.orientation.z;
        data_arr[3]=OdomMsg->pose.pose.orientation.w;
        
    }else{
        collectionFlag=0;
    }
}

int main(int argc, char **argv)
{

    fout.open(filename.c_str(), ios::out | ios::app);
    fout << "Pitch" << "," << "Roll" << "," << "pos_x" << "," << "pos_y" << "," << "yaw" << "," << "ori_w" << "\n";
    
    ros::init(argc, argv, "listener");
    ros::NodeHandle n;
    ros::Subscriber subData = n.subscribe("data_pub", 1000, chatterCallback);
    ros::Subscriber subOdom = n.subscribe("odom", 10, odomCallback);
    ros::Publisher dataFlag = n.advertise<std_msgs::Int32>("collectionFlag", 100);
    
    ros::Rate loop_rate(10); //running at 10Hz
    ros::spinOnce();

    while (ros::ok()) {
        
        std_msgs::Int32 flag; //0-moving (do nothing), 1-collect data (stopped), 2-compute data and send back(which will then turn back to 0)
        flag.data = collectionFlag;
        dataFlag.publish(flag);

        ros::spinOnce();
        loop_rate.sleep();
    }
    
    fout.close();
    return 0;
}

