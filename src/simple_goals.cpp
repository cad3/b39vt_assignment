#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>


        

	
	bool ob;
	

	void moveForward();
	void scanCallback();
	
	
	ros::Publisher operatorPublisher;
	float x_value = .25;



void scanCallback (const sensor_msgs::LaserScan::ConstPtr& scan_msg)
 	  {

 		
 		std::cout << "In scan call back" << std::endl;
 		//intializing variable to change robot's velocity
 
 		//setting minium distance to avoid object
 		double minDist = .6;
 

 		//go through all possible ranges and see if there are obstacles
 		for (int i=0; i < (scan_msg->ranges.size()); i++)
  			{   
 	 			if((scan_msg->ranges[i]) <= minDist)
 				{
         			   	ob = true;
         			   	std::cout << "The robot should stop!!!!!!!" << std::endl; 
 				}
 			else 
				{
            				ob = false;
            				std::cout << "The robot should be going at the set velocity" << std::endl;  
				}
			}	
	   }



void moveForward()
{
	geometry_msgs::Twist twist;
	

	if(ob == false)
	{ 	

          std::cout << "the robot should be moving" << std::endl;      

	  //twist.linear.x = x_value;
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
