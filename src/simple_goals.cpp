#include <stdlib.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

class TurtleOperator
{

public:
TurtleOperator();
virtual ~TurtleOperator() {};

void moveForward();

private:
ros::NodeHandle nodeHandle;
ros::Publisher operatorPublisher;
float x_value;

};

int main(int argc, char **argv)
{
ros::init(argc, argv, "move_turtle");

TurtleOperator turtleOperator;

std::cout << "Starting to spin …" << std::endl;

ros::Rate loopRate(15);

while(ros::ok())
{
turtleOperator.moveForward();
ROS_INFO("Published twist");
loopRate.sleep();
}

return EXIT_SUCCESS;
}
TurtleOperator::TurtleOperator()
: x_value(0.25)
{
operatorPublisher = nodeHandle.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1);
}

void TurtleOperator::moveForward()
{
geometry_msgs::Twist twist;

twist.linear.x = x_value;
twist.linear.y = 0;
twist.linear.z = 0;

twist.angular.x = 0;
twist.angular.y = 0;
twist.angular.z = 0;

this->operatorPublisher.publish(twist);
}
