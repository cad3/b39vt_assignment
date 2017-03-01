#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <stdio.h>
#include <math.h>

void moveForward();
void scanCallback();


ros::Publisher operatorPublisher;


float normal_speed = .09;
float turn_speed = .15;
float left = .6;
float right= -.6;

double minDist = .35;

bool obi;
int i;
double disti;
double totali;
double averagei;

bool obj;
int j;
double distj;
double totalj;
double averagej;

bool obk;
int k;
double distk;
double totalk;
double averagek;


void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg)
{
    //std::cout << "In scan call back" << std::endl;
    //intializing variable to change robot's velocity

    //setting minium distance to avoid object

    double lasrange = (scan_msg->ranges.size());

    //go through all possible ranges and see if there are obstacles

    disti = 0;
    totali =0;
    averagei = 0;



    distj = 0;
    totalj =0;
    averagej = 0;


    distk =0;
    totalk = 0;
    averagek = 0;

    for (i=0; i < (lasrange/3); i++)
    {
        disti = (scan_msg->ranges[i]);

     if(!isnan(disti) && !isinf(disti))
     {

        totali += disti;
       // std::cout << "this is totali " << totali << std::endl;

    }
    }
    //std::cout << "totali outside of loop " << totali << std::endl;
    std::cout << "totali average: " << totali/(lasrange/3) << std::endl;
    //std::cout << "lasrange/3: " << lasrange/3 << std::endl;

    averagei = totali/(lasrange/3);

    if(( averagei <= minDist && !isnan(totali) && !isinf(totali)))
    {
        //std::cout << "toali average: " << totali/(lasrange/3) << std::endl;
        obi = true;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;


    }
    if(( averagei >= minDist && !isnan(totali) && !isinf(totali)))
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

    std::cout << "totalj average: " << totalj/(lasrange/3) << std::endl;

    averagej = totalj/(lasrange/3);

    if(( averagej <= minDist && !isnan(totalj) && !isinf(totalj)))
    {
       // std::cout << "section 2 obstacle" << std::endl;
        obj = true;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;


    }
    if(( averagej >= minDist && !isnan(totalj) && !isinf(totalj)))
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

    std::cout << "totalk average: " << totalk/(lasrange/3) << std::endl;

    averagek = totalk/(lasrange/3);
    if( (averagek <= minDist && !isnan(totalk) && !isinf(totalk)))
    {
        obk = true;
        //std::cout << "section 3 obstacle" << std::endl;
        //std::cout << "The robot should stop!!!!!!!" << std::endl;

    }
    if(( averagek >= minDist && !isnan(totalk) && !isinf(totalk)))
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

    std::cout << "obi:     " << obi << std::endl;
    std::cout << "obj:     " << obj << std::endl;
    std::cout << "obk:     " << obk << std::endl;


    if((obj == true) && (obi == true) && (obk == true))
    {

        if(averagei >= averagej && averagei >= averagek)
        {
            std::cout << "turning right" << std::endl;

            twist.linear.x = turn_speed;
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = right;

            operatorPublisher.publish(twist);
        }

        if(averagej >= averagei && averagej >= averagek)
        {
            std::cout << "go backwards" << std::endl;

            twist.linear.x = -(normal_speed);
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = 0;

            operatorPublisher.publish(twist);
        }

        if(averagek >= averagei && averagek >= averagej)
        {
            std::cout << "go left" << std::endl;

            twist.linear.x = turn_speed;
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = left;

            operatorPublisher.publish(twist);
        }

    }

    if((obi == false) && (obj == true) && (obk == false) )
    {

        if(averagei >= averagek){
        std::cout << "turning right" << std::endl;

        twist.linear.x = turn_speed;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = right;

        operatorPublisher.publish(twist);
        }

        else {
            twist.linear.x = turn_speed;
            twist.linear.y = 0;
            twist.linear.z = 0;

            twist.angular.x = 0;
            twist.angular.y = 0;
            twist.angular.z = left;

            operatorPublisher.publish(twist);
        }
    }

    if((obi == true) && (obj == false) && (obk == true) )
    {

        std::cout << "moving forward " << std::endl;

        twist.linear.x = normal_speed;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 0;

        operatorPublisher.publish(twist);
    }

    if((obi == true) && (obj == false) && (obk == false))
    {

        std::cout << "moving left" << std::endl;

        twist.linear.x = turn_speed;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = left;

        operatorPublisher.publish(twist);
    }

    if((obi == false) && (obj == true) && (obk == true))
    {

        std::cout << "moving right" << std::endl;

        twist.linear.x = turn_speed;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = right;

        operatorPublisher.publish(twist);
    }



    if((obi == true) && (obj == true)  && (obk == false) )
    {

        std::cout << "going left " << std::endl;

        twist.linear.x = 0;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = left;

        operatorPublisher.publish(twist);
    }



    if((obi == false) && (obj == false) && (obk == true))
    {

        std::cout << "moving right " << std::endl;

        twist.linear.x = turn_speed;
        twist.linear.y = 0;
        twist.linear.z = 0;

        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = right;

        operatorPublisher.publish(twist);

    }

    if((obi == false) && (obj == false) && (obk == false))
    {

        std::cout << "moving straight " << std::endl;

        twist.linear.x = normal_speed;
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

    operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1000);

    ros::Subscriber scan_sub = nodeHandle.subscribe("scan", 1000, scanCallback);

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
