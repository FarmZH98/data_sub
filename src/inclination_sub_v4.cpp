//v4 is to 1)redo the coordinates of map (not using odom as it is not reflective towards coordinates in map)
//2) change subscribed topic to move_base/goal
//3) use get_yaw function to get yaw angle
#include "ros/ros.h"
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int32.h>
//#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>
#include <tf/transform_listener.h>
#include <actionlib_msgs/GoalStatusArray.h>
#include <actionlib/client/simple_action_client.h>
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
double data_arr[4] = {0};


void chatterCallback(const std_msgs::Float32MultiArray::ConstPtr& msg)
{
    ROS_INFO("pitch: [%f], roll: [%f], pos_x: [%f], pos_y: [%f], ori_z: [%f], ori_w: [%f]", msg->data[0], msg->data[1], data_arr[0], 
        data_arr[1], data_arr[2], data_arr[3]); 

    fout << msg->data[0] << "," << msg->data[1] << "," << data_arr[0] << "," << data_arr[1]<< "," 
        << data_arr[2] << "," << data_arr[3] << "\n";

}

/*void odomCallback(const nav_msgs::Odometry::ConstPtr& OdomMsg)
{
    //ROS_INFO("Collecting OdomMsg");

    if(abs(OdomMsg->twist.twist.linear.x <=0.005) && abs(OdomMsg->twist.twist.linear.y <= 0.005) && abs(OdomMsg->twist.twist.angular.z <=0.005)){ //1. try to modify to roughly 0.05. 2. others that had the same prob: got if statement too. 3. delay
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
}*/

void statusCallBack(const actionlib_msgs::GoalStatusArray::ConstPtr &status) //https://qiita.com/nnn112358/items/d159204d565469f647bb
{
    int status_id = 0;
    //uint8 PENDING         = 0  
    //uint8 ACTIVE          = 1 
    //uint8 PREEMPTED       = 2
    //uint8 SUCCEEDED       = 3
    //uint8 ABORTED         = 4
    //uint8 REJECTED        = 5
    //uint8 PREEMPTING      = 6
    //uint8 RECALLING       = 7
    //uint8 RECALLED        = 8
    //uint8 LOST            = 9

    if (!status->status_list.empty()){
    actionlib_msgs::GoalStatus goalStatus = status->status_list[0];
    status_id = goalStatus.status;
    }

    if(status_id==1){
    collectionFlag=0; //移動中
    }else if((status_id==3)||(status_id==0)){
        collectionFlag=1;
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

            ROS_INFO("Got a transform! x = %lf, y = %lf, yaw = %lf", posX, posY, yaw); //test to see if this indeeds get the coordinates.
            data_arr[0] = posX;
            data_arr[1] = posY;
            data_arr[2] = yaw;
            data_arr[3] = 0; //rubbish val
            fout << 0 << "," << 0 << "," << data_arr[0] << "," << data_arr[1]<< "," << data_arr[2] << "," << data_arr[3] << "\n"; //extra line for testing

        }catch (tf::TransformException ex){
            ROS_ERROR("Nope! %s", ex.what());
        } 
    }

}

int main(int argc, char **argv)
{

    fout.open(filename.c_str(), ios::out | ios::app);
    fout << "Pitch" << "," << "Roll" << "," << "pos_x" << "," << "pos_y" << "," << "yaw" << "," << "ori_w" << "\n";
    
    ros::init(argc, argv, "listener");
    ros::NodeHandle n;

    ros::Subscriber subData = n.subscribe("data_pub", 1000, chatterCallback);
    //ros::Subscriber subOdom = n.subscribe("odom", 10, odomCallback);
    ros::Subscriber subStatus = n.subscribe<actionlib_msgs::GoalStatusArray>("/move_base/status", 1, &statusCallBack); //ori q_size=10, noticed a lot of delay 
    ros::Publisher dataFlag = n.advertise<std_msgs::Int32>("collectionFlag", 100);
    
    ros::Rate loop_rate(10); //running at 10Hz
    ros::spinOnce();

    while (ros::ok()) {

        /*tf::TransformListener listener;
        tf::StampedTransform transform;

        // get coordinates of robot wrt to map
        try{ //rosrun tf tf_echo /map /base_link
            listener.waitForTransform("/map", "/base_link", ros::Time(0), ros::Duration(10.0) ); //uncomment this if error "Lookup would require extrapolation into the past" while running
            listener.lookupTransform("/map","/base_link",ros::Time(0), transform);
            ROS_INFO("Got a transform! x = %f, y = %f",transform.getOrigin().x(),transform.getOrigin().y()); //test to see if this indeeds get the coordinates.
            tf::Quaternion q = transform.getRotation(); 
            double yaw = tf::getYaw(q);
            double posX = transform.getOrigin().x();
            double posY = transform.getOrigin().y();
            data_arr[0] = posX;
            data_arr[1] = posY;
            data_arr[2] = yaw;
            data_arr[3] = 0; //rubbish val

        }catch (tf::TransformException ex){
            ROS_ERROR("Nope! %s", ex.what());
        } */
        
        //publish flag for sensor to collect & compute data
        std_msgs::Int32 flag; //0-moving (do nothing), 1-collect data (stopped), 2-compute data and send back(which will then turn back to 0)
        flag.data = collectionFlag;
        dataFlag.publish(flag);

        ros::spinOnce();
        loop_rate.sleep();
    }
    
    fout.close();
    return 0;
}

