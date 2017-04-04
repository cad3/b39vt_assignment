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
ros::Publisher led2;
ros::Publisher led1;

geometry_msgs::Twist twist;
kobuki_msgs::Led ledd;


float normal_speed = .1; //declaring speed of robot, if its not turning
float turn_speed = .08;    //declaring speed of robot, if it's turning
float left = .95;          //speed of rotation, if turning left
float right= -.95;         //speed of rotation, if turning right

double minDist = .45;      //minium distance for the robot to avoid obstacles

bool obi;                 //boolean value which declares whether or not an object is in the i section of laserscan
double disti;             //disti = ranges[i] in for loop where i is incrementing
double totali;            //totali is all instances of disti added together unless nan or infinity
double averagei;          //the average is calculated using the totali and ranges.size()/3

bool obj;
double distj;
double totalj;
double averagej;

bool obk;
double distk;
double totalk;
double averagek;

void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg) //method to tell the robot where the obstacles are
{
    double lasrange = (scan_msg->ranges.size());  //lasrange is a variable which stores ranges.size()
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

void moveForward()     //method that sets the velocity of the robot
{
    //declaring variable to switch velocity
    //lights
    //if there's obstacles on all sides
    if((obj == true) && (obi == true) && (obk == true))
    {

        std::cout << "obstacles everywhere" << std::endl;

        if(averagei >= averagej && averagei >= averagek)  //if obstacle furthest from robot is in section i
        {
            //robot moving & turning right
            twist.linear.x = turn_speed;
            twist.angular.z = right;

            ledd.value = 3;
            led1.publish(ledd);
            ledd.value = 0;
            led2.publish(ledd);
        }

        if(averagej >= averagei && averagej >= averagek)  //if obstacle furthest from robot is in section j
        {
            //robot going backwards
            twist.linear.x = -(normal_speed);
            twist.angular.z = 0;

            ledd.value = 0;
            led1.publish(ledd);
            led2.publish(ledd);
        }

        if(averagek >= averagei && averagek >= averagej)  //if obstacle furthest from robot is in section k
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

    if((obi == false) && (obj == true) && (obk == false) )
    {

        std::cout << "obstacle in front" << std::endl;
        if(averagei >= averagek)
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

    if((obi == true) && (obj == false) && (obk == true) )
    {

        //robot just moving forward
        twist.linear.x = normal_speed;
        twist.angular.z = 0;

        ledd.value = 0;
        led1.publish(ledd);
        led2.publish(ledd);

    }

    if((obi == true) && (obj == false) && (obk == false))
    {

        //robot moving and turning left
        twist.linear.x = turn_speed;
        twist.angular.z = left;

        ledd.value = 1;
        led2.publish(ledd);
        ledd.value = 0;
        led1.publish(ledd);

    }

    if((obi == false) && (obj == true) && (obk == true))
    {

        //robot moving & turning right
        twist.linear.x = turn_speed;
        twist.angular.z = right;

        ledd.value = 3;
        led1.publish(ledd);
        ledd.value = 0;
        led2.publish(ledd);

    }

    if((obi == true) && (obj == true)  && (obk == false) )
    {

        twist.linear.x = 0;
        twist.angular.z = left;

        ledd.value = 1;
        led2.publish(ledd);
        ledd.value = 0;
        led1.publish(ledd);

    }

    if((obi == false) && (obj == false) && (obk == true))
    {
        twist.linear.x = turn_speed;
        twist.angular.z = right;

        ledd.value = 3;
        led1.publish(ledd);
        ledd.value = 0;
        led2.publish(ledd);

    }

    if((obi == false) && (obj == false) && (obk == false))
    {
        twist.linear.x = normal_speed;
        twist.angular.z = 0;

        ledd.value = 0;
        led1.publish(ledd);
        led2.publish(ledd);

    }

    operatorPublisher.publish(twist);
}

int main(int argc, char **argv)
{
     ROS_INFO("UPDATTTTTING");
    //declaring node
    ros::init(argc, argv, "move_turtle");
    ros::NodeHandle nodeHandle;

    //set publisher which determines velocity of the robot
    operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1000);

    led2 = nodeHandle.advertise<kobuki_msgs::Led>("/mobile_base/commands/led2",1000);
    led1 = nodeHandle.advertise<kobuki_msgs::Led>("/mobile_base/commands/led1",1000);

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
