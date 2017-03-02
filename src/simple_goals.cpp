#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <stdio.h>
#include <math.h>
#include <kobuki_msgs/Led.h>



//initiating methods
void moveForward();
void scanCallback();

//declaring publisher
ros::Publisher operatorPublisher;
ros::Publisher left_light;
ros::Publisher right_light;

geometry_msgs::Twist twist;
kobuki_msgs::Led ledd;

//declaring speed of robot, if its not turning
float normal_speed = .09;
//declaring speed of robot, if it's turning
float turn_speed = .15;
//speed of rotation, if turning left
float left = .6;
//speed of rotation, if turning right
float right= -.6;

//minium distance for the robot to avoid obstacles
double minDist = .35;

//boolean value which declares whether or not an object is in the i section of laserscan
bool obi;
//disti = ranges[i] in for loop where i is incrementing
double disti;
//totali is all instances of disti added together unless nan or infinity
double totali;
//the average is calculated using the totali and ranges.size()/3
double averagei;

bool obj;
double distj;
double totalj;
double averagej;

bool obk;
double distk;
double totalk;
double averagek;

//method to tell the robot where the obstacles are
void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg)
{

    //lasrange is a variable which stores ranges.size()
    double lasrange = (scan_msg->ranges.size());

    //resetting values to zero
    disti = 0;
    totali =0;
    averagei = 0;

    distj = 0;
    totalj =0;
    averagej = 0;

    distk =0;
    totalk = 0;
    averagek = 0;

    //for loop to calculate total number of distances in the first third of the array ranges from LaserScan
    for (int i=0; i < (lasrange/3); i++)
    {
        disti = (scan_msg->ranges[i]);

        //conditional statements to make sure infinity and nan aren't added into the totali found
        if(!isnan(disti) && !isinf(disti))
        {
            totali += disti;
        }
    }

    //average distance in section i
    averagei = totali/(lasrange/3);

   //if averagei is less or equal to the minDist then there's an obstacle
    if(averagei <= minDist)
    {
        obi = true;
    }

    //if averagi is greater or equal to the minDist then there's NO obstacle
    if(averagei >= minDist)
    {   
        obi = false;
    }

    //same approach as used for i above but with the second two thirds of ranges[] looked at (j)
    for (int j=lasrange/3; j < 2*(lasrange/3); j++)
    {
        distj = scan_msg->ranges[j];

        if(!isnan(distj) && !isinf(distj))
        {
            totalj += distj;
        }
     }

    averagej = totalj/(lasrange/3);

    if(averagej <= minDist)
    {
        obj = true;
    }

    if(averagej >= minDist)
    { 
        obj = false;  
    }

    //same approach as used for j above but with last third of ranges[] looked at (k)
    for (int k=2*(lasrange/3); k<= lasrange; k++)
    {
        distk = (scan_msg->ranges[k]);

        if(!isnan(distk) && !isinf(distk))
        {
            totalk += distk;
        }
    }

    averagek = totalk/(lasrange/3);

    if(averagek <= minDist)
    {
        obk = true;       
    }

    if(averagek >= minDist)
    {
        obk = false;
    }

}

//method that sets the velocity of the robot
void moveForward()
{
    //declaring variable to switch velocity

    //lights

    //if there's obstacles on all sides
    if((obj == true) && (obi == true) && (obk == true))
    {
        //if obstacle furthest from robot is in section i
        if(averagei >= averagej && averagei >= averagek)
        {
            //robot moving & turning right
            twist.linear.x = turn_speed;

            twist.angular.z = right;

            //led1 light green
            ledd.value = 1;
            left_light.publish(ledd);
            //led2 light off
            ledd.value = 0;
            right_light.publish(ledd);

            operatorPublisher.publish(twist);
        }

        //if obstacle furthest from robot is in section j
        if(averagej >= averagei && averagej >= averagek)
        {
            //robot going backwards
            twist.linear.x = -(normal_speed);

            twist.angular.z = 0;

            //both led1 & led2 are off
            ledd.value = 0;
            left_light.publish(ledd);
            right_light.publish(ledd);

            operatorPublisher.publish(twist);
        }

        //if obstacle furthest from robot is in section k
        if(averagek >= averagei && averagek >= averagej)
        {
            //robot moving and going left
            twist.linear.x = turn_speed;

            twist.angular.z = left;

            //led2 is red
            ledd.value = 3;
            right_light.publish(ledd);
            //led1 is off
            ledd.value = 0;
            left_light.publish(ledd);

            operatorPublisher.publish(twist);
        }

    }

    if((obi == false) && (obj == true) && (obk == false) )
    {

        if(averagei >= averagek)
        {

            //robot moving & turning right
             twist.linear.x = turn_speed;

             twist.angular.z = right;

             //led1 green
             ledd.value = 1;
             left_light.publish(ledd);
             //led2 off
             ledd.value = 0;
             right_light.publish(ledd);

            operatorPublisher.publish(twist);
        }

        else
        {
            //robot moving & turning left
            twist.linear.x = turn_speed;

            twist.angular.z = left;

            //led2 red
            ledd.value = 3;
            right_light.publish(ledd);
            //led1 off
            ledd.value = 0;
            left_light.publish(ledd);

            operatorPublisher.publish(twist);
        }
    }

    if((obi == true) && (obj == false) && (obk == true) )
    {
        //robot just moving forward
        twist.linear.x = normal_speed;

        twist.angular.z = 0;

        //led1 & led2 off
        ledd.value = 0;
        left_light.publish(ledd);
        right_light.publish(ledd);

        operatorPublisher.publish(twist);
    }

    if((obi == true) && (obj == false) && (obk == false))
    {
        //robot moving and turning left
        twist.linear.x = turn_speed;

        twist.angular.z = left;

        //led2 red && led1 off
        ledd.value = 0;
        right_light.publish(ledd);
        left_light.publish(ledd);

        operatorPublisher.publish(twist);
    }

    if((obi == false) && (obj == true) && (obk == true))
    {
        //robot moving & turning right
        twist.linear.x = turn_speed;

        twist.angular.z = right;

        //led1 green && led2 off
        ledd.value = 1;
        left_light.publish(ledd);
        ledd.value = 0;
        right_light.publish(ledd);

        operatorPublisher.publish(twist);
    }



    if((obi == true) && (obj == true)  && (obk == false) )
    {
        //robot just turning left
        twist.linear.x = 0;

        twist.angular.z = left;

        //led2 red && led1 off
        ledd.value = 3;
        right_light.publish(ledd);
        ledd.value = 0;
        left_light.publish(ledd);

        operatorPublisher.publish(twist);
    }



    if((obi == false) && (obj == false) && (obk == true))
    {
        //robot moving and turning right
        twist.linear.x = turn_speed;

        twist.angular.z = right;

        //led1 green && led2 off
        ledd.value = 1;
        left_light.publish(ledd);
        ledd.value = 0;
        right_light.publish(ledd);

        operatorPublisher.publish(twist);
    }

    if((obi == false) && (obj == false) && (obk == false))
    {
        //robot moving in a straight line
        twist.linear.x = normal_speed;

        twist.angular.z = 0;

        //led1 & led2 off
        ledd.value = 0;
        left_light.publish(ledd);
        right_light.publish(ledd);

        operatorPublisher.publish(twist);
    }

}


int main(int argc, char **argv)
{

    //declaring node
    ros::init(argc, argv, "move_turtle");

    ros::NodeHandle nodeHandle;

    //set publisher which determines velocity of the robot
    operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1000);

    left_light = nodeHandle.advertise<kobuki_msgs::Led>("/mobile_base/commands/led2",1000);

    right_light = nodeHandle.advertise<kobuki_msgs::Led>("/mobile_base/commands/led1",1000);

    //subscribe to topic scan and input into method scanCallback
    ros::Subscriber scan_sub = nodeHandle.subscribe("scan", 1000, scanCallback);

    ros::Rate loopRate(15);

    while(ros::ok())
    {
        moveForward();
        ROS_INFO("Published Twist");
        ROS_INFO("Published Led");
        loopRate.sleep();
        ros::spinOnce();
    }

    return EXIT_SUCCESS;
}
