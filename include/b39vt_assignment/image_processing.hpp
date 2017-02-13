#ifndef IMAGE_PROCESSING_B39VT_ASSIGNMENT_HPP
#define IMAGE_PROCESSING_B39VT_ASSIGNMENT_HPP

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
/*cv::Mat result;
char* image_window = "Source Image";
char* result_window = "Result window";


int match_method; */

void templateMatching(const cv::Mat& im, const cv::Mat& templ,double threshold,int typePic);
// Your code goes here 

# endif // IMAGE_PROCESSING_B39VT_ASSIGNMENT_HPP
