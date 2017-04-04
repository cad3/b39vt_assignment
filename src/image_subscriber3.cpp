#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <std_msgs/String.h>
#include "ros/ros.h"
#include <tf/tf.h>
#include <sensor_msgs/LaserScan.h>
#include "b39vt_assignment/image_processing.hpp"
#include <geometry_msgs/PointStamped.h>
#include <tf/transform_listener.h>





class ImageSubscriber
{

     //transform code




    tf::TransformListener listener;



	int count;
	double distj;
    double totalj;
    double averagej;
    
    ros::Publisher mapPublish;
    ros::Publisher basePublish;

    ros::Publisher aliveMarking;
    ros::Publisher deadMarking;
    ros::Publisher smokingMarking;
    ros::Publisher biohazardMarking;
    ros::Publisher dangerMarking;
    ros::Publisher nuclearMarking;
    ros::Publisher toxicMarking;
    ros::Publisher fireMarking;
	bool marking;
	int pictureType;
	double thresholdMatch;
	// By default, variables and methods are private
	ros::NodeHandle nh_;
	image_transport::ImageTransport it_;
	image_transport::Subscriber image_sub_;
	double distTotal;
    double dist;
    geometry_msgs::PointStamped p;

    ros::Subscriber scanSub_;


  
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
            image_sub_ = it_.subscribe("/camera/rgb/image_raw", 1,
     		 &ImageSubscriber::imageCb, this);
        scanSub_ = nh_.subscribe<sensor_msgs::LaserScan>("/scan",1,&ImageSubscriber::processLaserScan,this);

        mapPublish = nh_.advertise<geometry_msgs::PoseStamped>("transformed_map", 10);
        basePublish = nh_.advertise<geometry_msgs::PoseStamped>("base", 10);

        aliveMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_alive",10);
        deadMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_dead",20);
        nuclearMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_nuclear",30);
        smokingMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_smoking",20);
        dangerMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_danger",30);
        biohazardMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_biohazard",20);
        toxicMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_poison",10);
        fireMarking = nh_.advertise<geometry_msgs::PointStamped>("match_location_fire",50);
   			// cv::namedWindow(OPENCV_WINDOW);
  		}

  		~ImageSubscriber()
  		{
    		cv::destroyWindow(OPENCV_WINDOW);
  		}

        void setTransform(geometry_msgs::PoseStamped pBase, geometry_msgs::PoseStamped pMap, ros::Time current_transform)
        {
        pBase.header.frame_id = "/base_link";
        pBase.pose.position.x = 0.0;
        pBase.pose.position.y = 0.0;
        pBase.pose.orientation = tf::createQuaternionMsgFromYaw(0.0);

        listener.getLatestCommonTime(pBase.header.frame_id, "map", current_transform, NULL);
        pBase.header.stamp = current_transform;
        ROS_INFO_STREAM("TRANSFORMING FROM BASE_LINK TO MAP");
        listener.transformPose("map", pBase, pMap);

        basePublish.publish(pBase);
        mapPublish.publish(pMap);

        // pMap now contains the pose of the robot transformed into map
        // coordinates according to the TF data available at time "current_transform"

        //end of transform code
        }


        ros::Publisher getAlivePublisher(){
            return aliveMarking;
        }
        
        ros::Publisher getDeadPublisher(){
            return deadMarking;
        }
        
        ros::Publisher getToxicPublisher(){
            return toxicMarking;
        }
        
        ros::Publisher getSmokingPublisher(){
            return smokingMarking;
        }
        
        ros::Publisher getNuclearPublisher(){
            return nuclearMarking;
        }
        
        ros::Publisher getFirePublisher(){
            return fireMarking;
        }
        
        ros::Publisher getBiohazardPublisher(){
        	return biohazardMarking;
        }
        
        ros::Publisher getDangerPublisher(){
            return dangerMarking;
        }

        geometry_msgs::PointStamped getPointStamped(){
            return p;
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
		void setMarking(bool templateFound)
		{
			marking = templateFound;
		}
		bool getMarking()
		{
			return marking;
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



    void processLaserScan (const sensor_msgs::LaserScanConstPtr& scan)
		{
		//std::cout << "going through processLaserScan " << "\n";
		double lasrange = (scan->ranges.size());
		
		  distj = 0;
    	  totalj =0;
          averagej = 0;
          
          
          
           //same approach as used for i above but with the second two thirds of ranges[] looked at (j)
    	for (int j=lasrange/3; j < 2*(lasrange/3); j++)
    	{
        	distj = scan->ranges[j];

        	if(!isnan(distj) && !isinf(distj))
        	{
        		//std::cout << "Not nan " << "\n";
            	totalj += distj;
        	}
        	
        	
     	}

    	averagej = totalj/(lasrange/3);
          
		setDistance(averagej);

		}
		
        //SETS POINT WHERE IMAGE FOUND IN TERMS OF BASELINK (STILL HAS TO BE CONVERTED TO MAP)
   		void setDistance(double disti)
		{
			dist = disti;
			p.header.frame_id = "base_link";
			p.point.x = dist;
			p.point.y = 0;
			p.point.z = 0;

			
		}
		
		double getDistance(){
		   return dist; 
         }

};
		

int main(int argc, char** argv)
{

            //tf::TransformListener listener;
            //ros::init(argc, argv, "subscriber");
            //ros::NodeHandle nh_;



  			cv::String dangerString = "/home/turtlebot/danger.png";
  

 			cv::String toxicString = "/home/turtlebot/toxic.png";
  
  			cv::String aliveString = "/home/turtlebot/darkalive.png";
  
  			cv::String deadString = "/home/turtlebot/dead.png";
  
  			cv::String smokingString = "/home/turtlebot/smoking.png";
  
  			cv::String nuclearString = "/home/turtlebot/nuclear.png";
  
  			cv::String fireString = "/home/turtlebot/fire.png";
  
  			cv::String biohazardString = "/home/turtlebot/biohazard.png";
  
  
  			cv::String imagess[8] = {dangerString,toxicString,aliveString,deadString,smokingString,nuclearString,fireString,biohazardString};
  
  
  			ros::init(argc, argv, "image_subscriber");

            geometry_msgs::PoseStamped pBase, pMap;


  			
  			ImageSubscriber ic;
 

  
  

  			while (ros::ok())
  			{
                 ROS_INFO_STREAM("AJ is the best");

                 ROS_INFO_STREAM("Better than all the rest");
                 ic.setTransform(pBase, pMap, ros::Time::now());
				if (ic.data_valid)
					{

                    // ros::Time current_transform = ros::Time::now();



      				
						for(int a =0; a < 8; a++)
						{
			
			//std::cout << "Matching Value = " << maxVal << "\n";
							cv::Mat tempp = cv::imread( imagess[a]);
							
							cv::resize(tempp, tempp, cv::Size(100,100));
		
								if(imagess[a].compare(toxicString) == 0)
								{

									ic.setPicType(1);
                                    ic.setThreshold(0.35);
	    					    	//std::cout << "checking for toxic template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(dangerString) == 0)
     							{
     								
     								ic.setPicType(2);
                                    ic.setThreshold(0.4);
	    							//std::cout << "checking for danger template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(aliveString) == 0)
     							{
     								ic.setPicType(3);
                                    ic.setThreshold(0.7);
	    							//std::cout << "checking for alive template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     	
     							if(imagess[a].compare(deadString) == 0)
     							{
     								ic.setPicType(4);
                                    ic.setThreshold(0.6);
	    							//std::cout << "checking for dead template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(smokingString) == 0)
     							{
     								ic.setPicType(5);
                                    ic.setThreshold(0.37); //0.37
	    							//std::cout << "checking for smoking template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(nuclearString) == 0)
     							{
     								ic.setPicType(6);
                                    ic.setThreshold(0.5);
	    							//std::cout << "checking for nuclear template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     							if(imagess[a].compare(fireString) == 0){
     								ic.setPicType(7);
                                    ic.setThreshold(0.5); //0.45
	    						    //std::cout << "checking for fire template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     	
     							if(imagess[a].compare(biohazardString) == 0)
     							{
     								ic.setPicType(8);
	     		 					ic.setThreshold(0.45);
	    						    std::cout << "checking for biohazard template & changing thresholdMatch" << ic.getThreshold() << "\n"; 
	
     							}
     	
     	
		 						//marking is set to testing
								ic.setMarking(templateMatching(ic.getImage(), tempp, ic.getThreshold(),ic.getPicType()));
							
							
								if(ic.getMarking() && ic.getPicType() == 1)
								{
								     
                                     ic.getToxicPublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found in toxic !!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 2)
								{
								     
                                     ic.getDangerPublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found in danger!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 3)
								{
								     
                                     ic.getAlivePublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found in alive!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 4)
								{
								     
                                     ic.getDeadPublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found dead!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 5)
								{
								     
                                     ic.getSmokingPublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found smoking!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 6)
								{
								     
                                     ic.getNuclearPublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found nuclear!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 7)
								{
								     
                                     ic.getFirePublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found in fire!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								if(ic.getMarking() && ic.getPicType() == 8)
								{
								     
                                     ic.getBiohazardPublisher().publish(ic.getPointStamped());
                                     
									std::cout << "Template found in biohazard!!!!!!! SO PUBLISHING!!!!! " << ic.getDistance() << "\n";

								}
								
								else
								{
									
									
									std::cout << "NO TEMPLATE FOUND: " << ic.getDistance() << "\n";
								}
     
       					}   
                 
				}
  				ros::spinOnce();
  	
  			}
  			return 0;
  }

