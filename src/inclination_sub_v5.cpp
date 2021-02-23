//v5 is to change status callback to use action client instead (which is done in ipa, so we just need to subscribe here
#include "ros/ros.h"
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
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
double data_arr[3] = {0};


void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg)
{
    ROS_INFO("pitch: [%f], roll: [%f], pos_x: [%f], pos_y: [%f], yaw: [%f]", msg->data[0], msg->data[1], data_arr[0], 
        data_arr[1], data_arr[2]); 

    fout << msg->data[0] << "," << msg->data[1] << "," << data_arr[0] << "," << data_arr[1]<< "," 
        << data_arr[2] << "\n";

}

void collectionCallback(const std_msgs::Int32::ConstPtr& msg) 
{
    if(msg->data==1){
        tf::TransformListener listener;
        tf::StampedTransform transform;

        // get coordinates of robot wrt to map origin
        try{
            listener.waitForTransform("/map","/base_link", ros::Time(0), ros::Duration(10.0) ); //uncomment this if error "Lookup would require extrapolation into the past" while running
            listener.lookupTransform("/map","/base_link",ros::Time(0), transform);
            tf::Quaternion q = transform.getRotation(); 
            double yaw = tf::getYaw(q);
            double posX = transform.getOrigin().x();
            double posY = transform.getOrigin().y();

            //ROS_INFO("Got a transform! x = %lf, y = %lf, yaw = %lf", posX, posY, yaw); //test to see if this indeeds get the coordinates.
            data_arr[0] = posX;
            data_arr[1] = posY;
            data_arr[2] = yaw;
            //fout << 0 << "," << 0 << "," << data_arr[0] << "," << data_arr[1]<< "," << data_arr[2] << "\n"; //extra line for testing

        }catch (tf::TransformException ex){
            ROS_ERROR("Nope! %s", ex.what());
        } 
    }

}

int main(int argc, char **argv)
{

    fout.open(filename.c_str(), ios::out | ios::app);
    fout << "Pitch" << "," << "Roll" << "," << "pos_x" << "," << "pos_y" << "," << "yaw" << "\n";
    
    ros::init(argc, argv, "listener");
    ros::NodeHandle n;

    ros::Subscriber subData = n.subscribe("data_pub", 1000, chatterCallback);
    ros::Subscriber subFlag = n.subscribe<std_msgs::Int32>("/room_exploration/room_exploration_server/collectionFlag", 100, collectionCallback);

    ros::spin();
    
    fout.close();
    return 0;
}

