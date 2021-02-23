#include "ros/ros.h"
#include "std_msgs/Float32.h"
#include <std_msgs/Float32MultiArray.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//bool even = true;

// create an ofstream for the file output (see the link on streams for
// more info)
fstream fout;
// create a name for the file output
std::string filename = "test.csv";

void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg)// can test if can add var and pass by reference
{
    ROS_INFO("I heard: [%f]  [%f]", msg->data.at(0), msg->data.at(1)); //not sure
    fout << msg->data.at(0) << "," << msg->data.at(0) << "\n";
    
}

int main(int argc, char **argv)
{

    fout.open(filename.c_str(), ios::out | ios::app);
    fout << "Pitch" << "," << "Roll" << "\n";

    ros::init(argc, argv, "listener");
    ros::NodeHandle n;
    ros::Subscriber sub1 = n.subscribe("data_pub", 1000, chatterCallback);
    //ros::Subscriber sub2 = n.subscribe("roll_pub", 1000, chatterCallback);

    ros::spin();
    //fout << "\n";

    // close the output file
    fout.close();
    return 0;
}

/*bool even = true;

// create an ofstream for the file output (see the link on streams for
// more info)
fstream fout;
// create a name for the file output
std::string filename = "test.csv";

void chatterCallback(const std_msgs::Float32::ConstPtr& msg)// can test if can add var and pass by reference
{
    ROS_INFO("I heard: [%f]", msg->data); //not sure
    if(!even)
        fout << msg->data << "\n";
    else
        fout << msg->data << ",";
    
    even=!even;
    
}

int main(int argc, char **argv)
{

    fout.open(filename.c_str(), ios::out | ios::app);
    fout << "Pitch" << "," << "Roll" << "\n";

    ros::init(argc, argv, "listener");
    ros::NodeHandle n;
    ros::Subscriber sub1 = n.subscribe("pitch_pub", 1000, chatterCallback);
    ros::Subscriber sub2 = n.subscribe("roll_pub", 1000, chatterCallback);

    ros::spin();
    //fout << "\n";

    // close the output file
    fout.close();
    return 0;
}*/

