#include "b39vt_assignment/image_processing.hpp"
#include <iostream>
#include <stdio.h>
#include <ros/ros.h>


bool templateMatching(const cv::Mat& img, const cv::Mat& templ,double threshold, int typePic)
{
    cv::Mat result;

	bool testing;
	
	int match_method=CV_TM_CCOEFF_NORMED;
	
	/// Source image to display
  	cv::Mat img_display;
  	img.copyTo( img_display );
	
  	/// Create the result matrix
  	int result_cols =  img.cols - templ.cols + 1;
  	int result_rows = img.rows - templ.rows + 1;

  	result.create( result_rows, result_cols, CV_32FC1 );

  	/// Do the Matching and Normalize
  	cv::matchTemplate( img, templ, result, match_method );

  	/// Localizing the best match with minMaxLoc
  	double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
  	cv::Point matchLoc;

 	cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

  	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
	{	
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;     
	}

    if(threshold > maxVal)
     {
		testing = false;
	 }
	
    else
     {
		testing = true;
     }

    if(testing == true)
      {

        if(typePic == 1)
         {
            std::cout << "Matching found in TOXIC " << maxVal << "\n";
         }
    
        if(typePic == 2)
         {
            std::cout << "Matching found in DANGER " << maxVal << "\n";
   		 }
    
         if(typePic == 3)
         {
            std::cout << "MATCH FOUND FOR ALIVE " << maxVal <<  "\n";
   		 }
    
        if(typePic == 4)
         {
            std::cout << "MATCH FOUND FOR DEAD " << maxVal <<  "\n";
         }
    
        if(typePic == 5)
        {
            std::cout << "MATCH FOUND FOR SMOKING" << maxVal << "\n";
    	}
    
        if(typePic == 6)
        {
            std::cout << "MATCH FOUND FOR NUCLEAR " << maxVal <<  "\n";
   		}
    
        if(typePic == 7)
        {
            std::cout << "MATCH FOUND FOR FIRE " << maxVal <<  "\n";
    	}

        if(typePic == 8)
         {
            std::cout << "MATCH FOUND FOR BIOHAZARD " << maxVal << "\n";
   		 }

 		/// Show me what you got
 		cv::rectangle( img_display, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows 	),   			cv::Scalar::all(0), 2, 8, 0 ); 
 	
 		cv::rectangle( result, matchLoc, cv::Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ),  		cv::Scalar::all(0), 2, 8, 0 );
 		
	}
	return testing;

}
