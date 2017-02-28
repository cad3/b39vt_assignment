#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <stdio.h>
#include <math.h>

bool obi;
bool obj;
bool obk;


void moveForward();
void scanCallback();


ros::Publisher operatorPublisher;
float x_value = .15;



void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg)
{
    //std::cout << "In scan call back" << std::endl;
    //intializing variable to change robot's velocity

    //setting minium distance to avoid object
    double minDist = 1.5;

    double lasrange = (scan_msg->ranges.size());

    //go through all possible ranges and see if there are obstacles
    int i;
    double disti = 0;
    double totali =0;
    int j;
    double distj = 0;
    double totalj =0;
    int k;
    double distk =0;
    double totalk = 0;

    for (i=0; i < (lasrange/3); i++)
    {
        disti = (scan_msg->ranges[i]);

     if(!isnan(disti) && !isinf(disti))
     {

        totali += disti;
        std::cout << "this is totali " << totali << std::endl;

    }
    }
    std::cout << "totali outside of loop " << totali << std::endl;
    //std::cout << "totali average: " << totali/(lasrange/3) << std::endl;
    //std::cout << "lasrange/3: " << lasrange/3 << std::endl;

    if(( totali/(lasrange/3) <= minDist && !isnan(totali) && !isinf(totali)))
    {
        //std::cout << "toali average: " << totali/(lasrange/3) << std::endl;
        obi = true;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;


    }
    if(( totali/(lasrange/3) >= minDist && !isnan(totali) && !isinf(totali)))
    {
        //std::cout << "section 1 NO obstacle" << std::endl;
        obi = false;

        //std::cout << "The robot should be going at the set velocity" << std::endl;
    }

    for (j=lasrange/3; j < 2*(lasrange/3); j++)
    {
        distj = scan_msg->ranges[j];
        if(!isnan(distj) && !isinf(distj))
        {

            totalj += distj;
           // std::cout << "this is totalj " << totalj << std::endl;
            //std::cout << "this is distj " << distj << std::endl;
        }
     }

    //std::cout << "totalj average: " << totalj/(lasrange/3) << std::endl;

    if(( totalj/(lasrange/3) <= minDist && !isnan(totalj) && !isinf(totalj)))
    {
       // std::cout << "section 2 obstacle" << std::endl;
        obj = true;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;


    }
    if(( totalj/(lasrange/3) >= minDist && !isnan(totalj) && !isinf(totalj)))
    {
        //std::cout << "section 2 NO obstacle" << std::endl;
        obj = false;
        //std::cout << "The robot should be going at the set velocity" << std::endl;
    }

    for (k=2*(lasrange/3); k<= lasrange; k++)
    {
        distk = (scan_msg->ranges[k]);
        if(!isnan(distk) && !isinf(distk))
        {

            totalk += distk;
            //std::cout << "this is totalk " << totalk << std::endl;
            //std::cout << "this is distk " << distk << std::endl;
        }
    }

    //std::cout << "totalk average: " << totalk/(lasrange/3) << std::endl;

    if( ( totalk/(lasrange/3) <= minDist && !isnan(totalk) && !isinf(totalk)))
    {
        obk = true;
        //std::cout << "section 3 obstacle" << std::endl;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;

    }
    if(( totalk/(lasrange/3) >= minDist && !isnan(totalk) && !isinf(totalk)))
    {
        //std::cout << "section 3 NO obstacle" << std::endl;
        obk = false;
        //std::cout << "The robot should be going at the set velocity" << std::endl;
    }
    //std::cout << ob << std::endl;

}
void moveForward()
{
    geometry_msgs::Twist twist;




    if(obj == false && obi == false && obk == false)
    {

        std::cout << "the robot should NOT be moving obstacles on all sides" << std::endl;

        twist.linear.x = 0;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 0.5;

        operatorPublisher.publish(twist);
    }

    if(obi == false && obj == true && obk == false )
    {

        std::cout << "the robot should be moving in j direction" << std::endl;

        twist.linear.x = -(x_value);
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 1.5;

        operatorPublisher.publish(twist);
    }

    if(obi == true && obk == false && obj == false )
    {

        std::cout << "the robot should be moving in the i direction" << std::endl;

        twist.linear.x = 0;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 0.5;

        operatorPublisher.publish(twist);
    }

    if(obi == false && obj == false && obk == true)
    {

        std::cout << "the robot should be moving in k direction" << std::endl;

        twist.linear.x = 0;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = -0.5;

        operatorPublisher.publish(twist);

    }

    if(obi == false && obj == false && obk == false)
    {

        std::cout << "the robot should be moving in a striaght line as no obstacles" << std::endl;

        twist.linear.x = x_value;
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

    operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1000);

    ros::Subscriber scan_sub = nodeHandle.subscribe("scan", 1000, scanCallback);


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
