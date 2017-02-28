#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <stdio.h>
#include <math.h>

bool ob;


void moveForward();
void scanCallback();


ros::Publisher operatorPublisher;
float x_value = .15;



void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg)
{
    //std::cout << "In scan call back" << std::endl;
    //intializing variable to change robot's velocity

    //setting minium distance to avoid object
    double minDist = 1;

    double lasrange = (scan_msg->ranges.size());

    //go through all possible ranges and see if there are obstacles
    int i =0;
    double totali =0;
    int j=0;
    double totalj =0;
    int k=0;
    double totalk = 0;

    for (i=0; i < lasrange/3; i++)
    {


     if(!isnan(totali) && !isinf(totali))
     {
        totali = totali + (scan_msg->ranges[i]);
        std::cout << "this is ranges[i]" << (scan_msg->ranges[i]) << std::endl;

    }
    }
    std::cout << "totali average: " << totali/(lasrange/3) << std::endl;
    std::cout << "lasrange/3: " << lasrange/3 << std::endl;

    if(( totali/(lasrange/3) <= minDist && !isnan(totali) && !isinf(totali)))
    {
        //std::cout << "toali average: " << totali/(lasrange/3) << std::endl;
        ob = true;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;
    }
    if(( totali/(lasrange/3) >= minDist && !isnan(totali) && !isinf(totali)))
    {
        //std::cout << "section 1 NO obstacle" << std::endl;
        ob = false;

        //std::cout << "The robot should be going at the set velocity" << std::endl;
    }

    for (j=lasrange/3; j < 2*(lasrange/3); j++)
    {
        if(!isnan(totalj) && !isinf(totalj))
        {
            totalj = totalj + (scan_msg->ranges[j]);
            //std::cout << "this is totalj " << totalj << std::endl;
            std::cout << "this is ranges[j]" << (scan_msg->ranges[j]) << std::endl;
        }
     }

    std::cout << "totalj average: " << totalj/(lasrange/3) << std::endl;

    if(( totalj/(lasrange/3) <= minDist && !isnan(totalj) && !isinf(totalj)))
    {
       // std::cout << "section 2 obstacle" << std::endl;
        ob = true;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;
    }
    if(( totalj/(lasrange/3) >= minDist && !isnan(totalj) && !isinf(totalj)))
    {
        //std::cout << "section 2 NO obstacle" << std::endl;
        ob = false;
        //std::cout << "The robot should be going at the set velocity" << std::endl;
    }

    for (k=2*(lasrange/3); k<= lasrange; k++)
    {
        if(!isnan(totalk) && !isinf(totalk))
        {
            totalk = totalk + (scan_msg->ranges[k]);
            //std::cout << "this is totalk " << totalk << std::endl;
            std::cout << "this is ranges[k]" << (scan_msg->ranges[k]) << std::endl;
        }
    }

    std::cout << "totalk average: " << totalk/(lasrange/3) << std::endl;

    if( ( totalk/(lasrange/3) <= minDist && !isnan(totalk) && !isinf(totalk)))
    {
        ob = true;
        //std::cout << "section 3 obstacle" << std::endl;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;
    }
    if(( totalk/(lasrange/3) >= minDist && !isnan(totalk) && !isinf(totalk)))
    {
        //std::cout << "section 3 NO obstacle" << std::endl;
        ob = false;
        //std::cout << "The robot should be going at the set velocity" << std::endl;
    }

}
void moveForward()
{
    geometry_msgs::Twist twist;


    if(ob == false)
    {

        std::cout << "the robot should be moving" << std::endl;

        twist.linear.x = x_value;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 0;

        operatorPublisher.publish(twist);
    }

    else
    {

        std::cout << "THE ROBOT SHOULD NOT BE MOVING!!!!" << std::endl;

        twist.linear.x = 0;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 0;

        operatorPublisher.publish(twist);

    }


}


int main(int argc, char **argv)
{


    ros::init(argc, argv, "move_turtle");

    ros::NodeHandle nodeHandle;

    //TurtleOperator turtleOperator;

    //added

    operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1);

    ros::Subscriber scan_sub = nodeHandle.subscribe("scan", 1, scanCallback);


    //added

    std::cout << "Starting to spin …" << std::endl;

    ros::Rate loopRate(15);

    while(ros::ok())
    {
        moveForward();
        ROS_INFO("Published twist");
        loopRate.sleep();
        ros::spinOnce();
    }

    return EXIT_SUCCESS;
}
