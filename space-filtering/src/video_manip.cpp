#include <algorithm>
#include <bits/stdint-uintn.h>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <cstdio>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

double alfa;
int alfa_slider = 0;
int alfa_slider_max = 100;

int top_slider = 0;
int top_slider_max = 100;

int height_slider = 50;
int height_slider_max = 100;

int width_slider = 30;
int width_slider_max = 100;

int intensity_slider = 100;
int intensity_slider_max = 500;

cv::Mat image1, image2, blended, mask;
cv::Mat imageTop, imageBottom;

char TrackbarName[50];

void modify_mask(cv::Mat &mask)
{
  int upper_step = 0;
  int lower_step = 0;
  int focus_width = width_slider*(mask.rows)/100;
  int focus_height = height_slider*(mask.rows)/100;
  float focus_intensity = 1.0*intensity_slider/100;
  cv::Mat(image1.rows, image1.cols, CV_8UC3, cv::Scalar(255, 255, 255)).copyTo(mask);
  std::vector<int> upper_val = {0, 0, 0};
  std::vector<int> lower_val = {255, 255, 255};

  // I used the matrix_data approach because I wanted to see if there was a noticeable difference
  // in speed by using that (in contrast with the at<> method).

  upper_step =  floor(255.0/(focus_height - focus_width/2.0))*focus_intensity;
  lower_step =  floor(255.0/(mask.rows - (focus_height + focus_width/2.0)))*focus_intensity;

  // The focus region has the following characterístics: it's centered around the focus_height and it has
  // the remaining of the image outside [focus_height - focus_width/2, focus_height + focus_width/2]
  // as a transition zone above and below it.

  int k = 0;
  for (int i=0; i<mask.rows; i++)
	{
	  // The loop actually goes 3 times through i = n, because the image
	  // has 3 color channels; hence, I need to reset the increment value each time,
	  // otherwise it will show stripes on screen due to the resetting of the uchar
	  // value inside the image.
	  if (i < std::max(focus_height - focus_width/2, 0))
		{
		  upper_val[0] += upper_step;
		  upper_val[1] += upper_step;
		  upper_val[2] += upper_step;
		}
	  if (i >= std::min(focus_height + focus_width/2, 255))
		{
		  lower_val[0] -= lower_step;
		  lower_val[1] -= lower_step;
		  lower_val[2] -= lower_step;
		}

	  for (int j=0; j<mask.cols; j++)
		{
		  if (i < std::max(focus_height - focus_width/2, 0))
			{
			  if (upper_val[0] <= 255)
				{
				  mask.at<cv::Vec3b>(i, j)[0] = upper_val[0];
				  mask.at<cv::Vec3b>(i, j)[1] = upper_val[1];
				  mask.at<cv::Vec3b>(i, j)[2] = upper_val[2];
				}
			  else
				{
				  mask.at<cv::Vec3b>(i, j)[0] = 255;
				  mask.at<cv::Vec3b>(i, j)[1] = 255;
				  mask.at<cv::Vec3b>(i, j)[2] = 255;
				}
			}
		  else if (i >= std::min(focus_height + focus_width/2, 255))
			{
			  if (lower_val[0] >= 0)
				{
				  mask.at<cv::Vec3b>(i, j)[0] = lower_val[0];
				  mask.at<cv::Vec3b>(i, j)[1] = lower_val[1];
				  mask.at<cv::Vec3b>(i, j)[2] = lower_val[2];
				}
			  else
				{
				  mask.at<cv::Vec3b>(i, j)[0] = 0;
				  mask.at<cv::Vec3b>(i, j)[1] = 0;
				  mask.at<cv::Vec3b>(i, j)[2] = 0;
				}
			}
		}
	}
}

void on_trackbar_blend(int, void*){
 alfa = (double) alfa_slider/alfa_slider_max ;
 cv::addWeighted(image1, 1-alfa, imageTop, alfa, 0.0, blended);
 cv::imshow("addweighted", blended);
}

void on_trackbar_line(int, void*){
  image1.copyTo(imageTop);
  int limit = top_slider*255/100;
  if(limit > 0){
    cv::Mat tmp = image2(cv::Rect(0, 0, 256, limit));
    tmp.copyTo(imageTop(cv::Rect(0, 0, 256, limit)));
  }
  on_trackbar_blend(alfa_slider,0);
}

void mask_control(int, void*)
{
  modify_mask(mask);
  cv::imshow("Mask", mask);
  imageBottom = image1.mul(mask*(1.0/255));//cv::multiply(mask, mask*(1.0/255), blended);

  // When using the cv::Mat::ones method only the first channel is initialized to 1,
  // therefore I need to initialize the other two myself. I used an ordinary initialization instead.

  imageTop = image1.mul(cv::Mat(mask.rows, mask.cols, CV_8UC3, CV_RGB(1, 1, 1)) - mask*(1.0/255));//cv::multiply(mask, mask*(1.0/255), blended);
  blended = imageTop + imageBottom;
  cv::imshow("addweighted", blended);
}


void average_filter(cv::Mat &src, cv::Mat &destination_img)
{
  cv::Mat average_mask = cv::Mat::ones(5, 5, CV_32F)*0.04;
  cv::filter2D(src, destination_img, src.depth(), average_mask, cv::Point(1, 1), 0);
}

cv::VideoCapture modify_video(cv::VideoCapture &original_video)
{
  
}

int main(int argvc, char** argv){
  // Creating a VideoCapture object to hold the video file.
  cv::VideoCapture video = cv::VideoCapture("./figures/olaf.mp4");

  if (!video.isOpened()) {
	std::cerr << "Couldn't open the video file. Exiting!" << std::endl;
	return -1;
  }
  
  // Here we'll loop through the video frames until the video file has been read in its entirety.
  
  while (1) {
	cv::Mat current_frame;

	video >> current_frame;

	if (current_frame.empty()) {
	  break;
	}

	// Display the current frame
	imshow("Frame", current_frame);
	
  }


  image1 = cv::imread("./figures/blend1.jpg");
  mask = cv::Mat(image1.rows, image1.cols, CV_8UC3, cv::Scalar(255, 255, 255));//CV_8UC1, cv::Scalar(255,255,255));

  // Creation of the averaging mask and its application upon
  // the original image.
  
  average_filter(image1, image2);

  image2.copyTo(imageTop);
  cv::namedWindow("addweighted", 1);
  cv::namedWindow("Mask", 1);

  std::sprintf( TrackbarName, "Alpha x %d", alfa_slider_max );
  cv::createTrackbar( TrackbarName, "addweighted",
                      &alfa_slider,
                      alfa_slider_max,
                      on_trackbar_blend );
  on_trackbar_blend(alfa_slider, 0 );

  std::sprintf( TrackbarName, "Scanline x %d", top_slider_max );
  cv::createTrackbar( TrackbarName, "addweighted",
                      &top_slider,
                      top_slider_max,
                      on_trackbar_line );
  on_trackbar_line(top_slider, 0 );

  std::sprintf( TrackbarName, "Height x %d", height_slider_max );
  cv::createTrackbar( TrackbarName, "Mask",
                      &height_slider,
                      height_slider_max,
                      mask_control);
  mask_control(height_slider, 0);

  std::sprintf( TrackbarName, "Width x %d", width_slider_max );
  cv::createTrackbar( TrackbarName, "Mask",
                      &width_slider,
                      width_slider_max,
                      mask_control);
  mask_control(width_slider, 0);

  std::sprintf( TrackbarName, "Intensity x %d", intensity_slider_max );
  cv::createTrackbar( TrackbarName, "Mask",
                      &intensity_slider,
                      intensity_slider_max,
                      mask_control);
  mask_control(intensity_slider, 0);

  char c = (char) cv::waitKey(25);
  if (c == 27)
	return 0;
  else if (c == 32)
	modify_video(video);
}
