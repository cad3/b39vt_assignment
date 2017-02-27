#include <ros/ros.h>
#include <sensor_msgs/LaserScan.h>
#include <geometry_msgs/Twist.h>

//LaserScan messages inputted into method scanCallback
void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg)
 {
 std::cout << "In scan call back" << std::endl;
 //intializing variable to change robot's velocity
 geometry_msgs::Twist vel; 
  //can't use a node handler
  //creating publisher to alter robot's velocity
 ros::Publisher vel_pub_=n.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 1);
 ros::Rate loop_rate(50);
  
 //setting minium distance to avoid object
 double minDist = .5;
 //setting velocity of robot when no obstacles
 double velocity = .5;

 //go through all possible ranges and see if there are obstacles
 for (int i=0; i < (scan_msg->ranges.size()); i++)
  	{   

 	if((scan_msg->ranges[i]) <= minDist)
 		{

            	vel.linear.x = 0;
            	vel.angular.z = 0;
            	vel_pub_.publish(vel);
            	std::cout << "The robot should stop!!!!!!!" << std::endl;
 		}
 	else 
		{


            	velocity=0.5*velocity;
            	vel.linear.x = velocity;
            	vel.angular.z = 0;
            	vel_pub_.publish(vel);
            	std::cout << "The robot should be going at the set velocity" << std::endl;  


		}

	}
 }


 
int main(int argc, char** argv)
 {
  ros::init(argc, argv, "Obstacle_Avoidance");
  ros::NodeHandle n;

  ros::Subscriber scan_sub = n.subscribe("scan", 1, scanCallback);
  ros::Publisher vel_pub_=n.advertise<geometry_msgs::Twist>("/mobile_base/commands/velocity", 1);
 
  std::cout << "In the main method" << std::endl;  

  ros::spin(); 
  return 0;

 }


