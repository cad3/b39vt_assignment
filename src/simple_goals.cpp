#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <stdio.h>
#include <math.h>
#include <kobuki_msgs/Led.h>

class SimpleGoals
{
//initiating methods
//void moveForward();
//void scanCallback();


//declaring publisher
ros::Subscriber scan_sub;
ros::Publisher operatorPublisher;
ros::Publisher led2;
ros::Publisher led1;

geometry_msgs::Twist twist;
kobuki_msgs::Led ledd;


const static float normal_speed = .1; //declaring speed of robot, if its not turning
const static float turn_speed = .08;    //declaring speed of robot, if it's turning
const static float left = .95;          //speed of rotation, if turning left
const static float right= -.95;         //speed of rotation, if turning right

const static double minDist = .45;      //minium distance for the robot to avoid obstacles

bool obright;                 //boolean value which declares whether or not an object is in the i section of laserscan
double distRight;             //disti = ranges[i] in for loop where i is incrementing
double totalRight;            //totali is all instances of disti added together unless nan or infinity
double averageRight;          //the average is calculated using the totali and ranges.size()/3

bool obMid;
double distMid;
double totalMid;
double averageMid;

bool obLeft;
double distLeft;
double totalLeft;
double averageLeft;

public:
SimpleGoals(){
    ros::NodeHandle nodeHandle;
    operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1000);

    led2 = nodeHandle.advertise<kobuki_msgs::Led>("/mobile_base/commands/led2",1000);
    led1 = nodeHandle.advertise<kobuki_msgs::Led>("/mobile_base/commands/led1",1000);

    //subscribe to topic scan and input into method scanCallback
   // scanSub_ = nh_.subscribe<sensor_msgs::LaserScan>("/scan",1,&ImageSubscriber::processLaserScan,this);

    scan_sub = nodeHandle.subscribe<sensor_msgs::LaserScan>("scan",1,&SimpleGoals::scanCallback,this);
}

ros::Publisher getPublisher()
{
    return operatorPublisher;
}

ros::Subscriber getSubscriber()
{
    return scan_sub;
}

ros::Publisher getLed1()
{
    return led1;
}

ros::Publisher getLed2()
{
    return led2;
}

void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg) //method to tell the robot where the obstacles are
{
    double lasrange = (scan_msg->ranges.size());  //lasrange is a variable which stores ranges.size()
    //resetting values to zero
    distRight = 0;
    totalRight =0;
    averageRight = 0;

    distMid = 0;
    totalMid =0;
    averageMid = 0;

    distLeft =0;
    totalLeft = 0;
    averageLeft = 0;

    //for loop to calculate total number of distances in the first third of the array ranges from LaserScan
    for (int i=0; i < (lasrange/3); i++)
    {
        distRight = (scan_msg->ranges[i]);

        //conditional statements to make sure infinity and nan aren't added into the totali found
        if(!isnan(distRight) && !isinf(distRight))
        {
            totalRight += distRight;
        }
    }

    //average distance in section i
    averageRight = totalRight/(lasrange/3);

   //if averagei is less or equal to the minDist then there's an obstacle
    if(averageRight <= minDist)
    {
        obright = true;
    }

    //if averagi is greater or equal to the minDist then there's NO obstacle
    if(averageRight >= minDist)
    {
        obright = false;
    }

    //same approach as used for i above but with the second two thirds of ranges[] looked at (j)
    for (int j=lasrange/3; j < 2*(lasrange/3); j++)
    {
        distMid = scan_msg->ranges[j];

        if(!isnan(distMid) && !isinf(distMid))
        {
            totalMid += distMid;
        }
     }

    averageMid = totalMid/(lasrange/3);

    if(averageMid <= minDist)
    {
        obMid = true;
    }

    if(averageMid >= minDist)
    {
        obMid = false;
    }

    //same approach as used for j above but with last third of ranges[] looked at (k)
    for (int k=2*(lasrange/3); k<= lasrange; k++)
    {
        distLeft = (scan_msg->ranges[k]);

        if(!isnan(distLeft) && !isinf(distLeft))
        {
            totalLeft += distLeft;
        }
    }

    averageLeft = totalLeft/(lasrange/3);

    if(averageLeft <= minDist)
    {
        obLeft = true;
    }

    if(averageLeft >= minDist)
    {
        obLeft = false;
    }

}

void moveForward()     //method that sets the velocity of the robot
{
    //declaring variable to switch velocity
    //lights
    //if there's obstacles on all sides
    if((obMid == true) && (obright == true) && (obLeft == true))
    {

        std::cout << "obstacles everywhere" << std::endl;

        if(averageRight >= averageMid && averageRight >= averageLeft)  //if obstacle furthest from robot is in section i
        {
            //robot moving & turning right
            twist.linear.x = turn_speed;
            twist.angular.z = right;

            ledd.value = 3;
            led1.publish(ledd);
            ledd.value = 0;
            led2.publish(ledd);
        }

        if(averageMid >= averageRight && averageMid >= averageLeft)  //if obstacle furthest from robot is in section j
        {
            //robot going backwards
            twist.linear.x = -(normal_speed);
            twist.angular.z = 0;

            ledd.value = 0;
            led1.publish(ledd);
            led2.publish(ledd);
        }

        if(averageLeft >= averageRight && averageLeft >= averageMid)  //if obstacle furthest from robot is in section k
        {
            //robot moving and going left
            twist.linear.x = turn_speed;
            twist.angular.z = left;

            ledd.value = 1;
            led2.publish(ledd);
            ledd.value = 0;
            led1.publish(ledd);
        }

    }

    if((obright == false) && (obMid == true) && (obLeft == false) )
    {

        std::cout << "obstacle in front" << std::endl;
        if(averageRight >= averageLeft)
        {
             //robot moving & turning right
             twist.linear.x = turn_speed;
             twist.angular.z = right;

             ledd.value = 3;
             led1.publish(ledd);
             ledd.value = 0;
             led2.publish(ledd);

        }

        else
        {
            //robot moving & turning left
            twist.linear.x = turn_speed;
            twist.angular.z = left;

            ledd.value = 1;
            led2.publish(ledd);
            ledd.value = 0;
            led1.publish(ledd);

        }
    }

    if((obright == true) && (obMid == false) && (obLeft == true) )
    {

        //robot just moving forward
        twist.linear.x = normal_speed;
        twist.angular.z = 0;

        ledd.value = 0;
        led1.publish(ledd);
        led2.publish(ledd);

    }

    if((obright == true) && (obMid == false) && (obLeft == false))
    {

        //robot moving and turning left
        twist.linear.x = turn_speed;
        twist.angular.z = left;

        ledd.value = 1;
        led2.publish(ledd);
        ledd.value = 0;
        led1.publish(ledd);

    }

    if((obright == false) && (obMid == true) && (obLeft == true))
    {

        //robot moving & turning right
        twist.linear.x = turn_speed;
        twist.angular.z = right;

        ledd.value = 3;
        led1.publish(ledd);
        ledd.value = 0;
        led2.publish(ledd);

    }

    if((obright == true) && (obMid == true)  && (obLeft == false) )
    {

        twist.linear.x = 0;
        twist.angular.z = left;

        ledd.value = 1;
        led2.publish(ledd);
        ledd.value = 0;
        led1.publish(ledd);

    }

    if((obright == false) && (obMid == false) && (obLeft == true))
    {
        twist.linear.x = turn_speed;
        twist.angular.z = right;

        ledd.value = 3;
        led1.publish(ledd);
        ledd.value = 0;
        led2.publish(ledd);

    }

    if((obright == false) && (obMid == false) && (obLeft == false))
    {
        twist.linear.x = normal_speed;
        twist.angular.z = 0;

        ledd.value = 0;
        led1.publish(ledd);
        led2.publish(ledd);

    }

    operatorPublisher.publish(twist);
}
};

int main(int argc, char **argv)
{
     ROS_INFO("UPDATTTTTING");
    //declaring node
    ros::init(argc, argv, "move_turtle");


    SimpleGoals sg;

    //set publisher which determines velocity of the robot

    ros::Rate loopRate(15);

    while(ros::ok())
    {
        sg.moveForward();
        ROS_INFO("Published Twist");
        ROS_INFO("Published Led");
        loopRate.sleep();
        ros::spinOnce();
    }

    return EXIT_SUCCESS;
}
