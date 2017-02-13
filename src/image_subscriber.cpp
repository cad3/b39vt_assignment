#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <std_msgs/String.h>


#include "b39vt_assignment/image_processing.hpp"

class ImageSubscriber
{

	int pictureType;
	double thresholdMatch;
	// By default, variables and methods are private
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_;
	image_transport::Subscriber image_sub_;
  
	cv_bridge::CvImagePtr cv_ptr;
  
  	const std::string OPENCV_WINDOW;
  
	public:
	// Public variables - it is a good practice to separate the declaration of
	// variables from the declaration/definition of methods
		bool data_valid;

	//added from processor
	public:
	// Public methods
  		ImageSubscriber() : it_(nh_), data_valid(false), OPENCV_WINDOW("Image window")
  		{
    	// Subscribe to input video feed
    		image_sub_ = it_.subscribe("/usb_cam/image_raw", 1, 
     		 &ImageSubscriber::imageCb, this);


   			// cv::namedWindow(OPENCV_WINDOW);
  		}

  		~ImageSubscriber()
  		{
    		cv::destroyWindow(OPENCV_WINDOW);
  		}

  		void imageCb(const sensor_msgs::ImageConstPtr& msg)
  		{
    		try
    		{
      			cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
      			data_valid = true;
    		}
    		catch (cv_bridge::Exception& e)
    		{
      			ROS_ERROR("cv_bridge exception: %s", e.what());
      			return;
    		}
    
  
 		 }


		cv::Mat getImage()
		{

			return cv_ptr->image;

		}
	
	
		void setThreshold(double threshold)
		{
	
	  		thresholdMatch = threshold;
		}
	
		double getThreshold()
		{
	
			return thresholdMatch;
			
		}
	
	 	void setPicType(int typee){
	
	    	pictureType = typee;
		}
	
		int getPicType()
		{
	 		return pictureType;
		}
};
		int main(int argc, char** argv)
		{


  			cv::String dangerString = "/home/turtlebot/danger.png";
  

 			cv::String toxicString = "/home/turtlebot/toxic.png";
  
  			cv::String aliveString = "/home/turtlebot/alive.png";
  
  			cv::String deadString = "/home/turtlebot/dead.png";
  
  			cv::String smokingString = "/home/turtlebot/smoking.png";
  
  			cv::String nuclearString = "/home/turtlebot/nuclear.png";
  
  			cv::String fireString = "/home/turtlebot/fire.png";
  
  			cv::String biohazardString = "/home/turtlebot/biohazard.png";
  
  
  			cv::String imagess[8] = {dangerString,toxicString,aliveString,deadString,smokingString,nuclearString,fireString,biohazardString};
  
  
  			ros::init(argc, argv, "image_subscriber");
  			
  			ImageSubscriber ic;
 

  
  

  			while (ros::ok())
  			{
				if (ic.data_valid)
					{
      				
						for(int a =0; a < 8; a++)
						{
			
			
							cv::Mat tempp = cv::imread( imagess[a]);
							cv::resize(tempp, tempp, cv::Size(130,130));
		
								if(imagess[a].compare(toxicString) == 0)
								{
		
									ic.setPicType(1);
	      							ic.setThreshold(0.31);
	    							std::cout << "checking for toxic template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(dangerString) == 0)
     							{
     								
     								ic.setPicType(2);
	      							ic.setThreshold(0.45);
	    							std::cout << "checking for danger template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(aliveString) == 0)
     							{
     								ic.setPicType(3);
	      							ic.setThreshold(0.385);
	    							std::cout << "checking for alive template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     	
     							if(imagess[a].compare(deadString) == 0)
     							{
     								ic.setPicType(4);
	      							ic.setThreshold(0.391);
	    							std::cout << "checking for dead template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(smokingString) == 0)
     							{
     								ic.setPicType(5);
	     							ic.setThreshold(0.3);
	    							std::cout << "checking for smoking template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(nuclearString) == 0)
     							{
     								ic.setPicType(6);
	      							ic.setThreshold(0.5);
	    							std::cout << "checking for nuclear template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(fireString) == 0){
     								ic.setPicType(7);
	     			 				ic.setThreshold(0.6);
	    							std::cout << "checking for fire template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     	
     							if(imagess[a].compare(biohazardString) == 0)
     							{
     								ic.setPicType(8);
	     		 					ic.setThreshold(0.4);
	    							std::cout << "checking for biohazard template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     	
		 
								templateMatching(ic.getImage(), tempp, ic.getThreshold(),ic.getPicType());
			
     
       					}   
                 
				}
  				ros::spinOnce();
  	
  			}
  			return 0;
  
}
