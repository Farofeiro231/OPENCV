#include <cmath>
#include <iostream>
#include <cstdio>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

double alfa;
int alfa_slider = 0;
int alfa_slider_max = 100;

int top_slider = 0;
int top_slider_max = 100;

cv::Mat image1, image2, blended;
cv::Mat imageTop;

char TrackbarName[50];

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


void average_filter(cv::Mat &src, cv::Mat &destination_img)
{
  cv::Mat average_mask = cv::Mat::ones(5, 5, CV_32F)*0.04;
  cv::filter2D(src, destination_img, src.depth(), average_mask, cv::Point(1, 1), 0);
}


int main(int argvc, char** argv){
  image1 = cv::imread("./figures/blend1.jpg");

  // Creation of the averaging mask and its application upon
  // the original image.
  
  float upper_increment = 0;
  float lower_increment = 0;
  int unfocus_width = 40;
  int unfocus_intensity = 100;
  cv::Mat alpha_matrix(image1.rows, image1.cols, CV_32F);//CV_8UC1, cv::Scalar(255,255,255));
  cv::imshow("Alpha matrix", alpha_matrix);
  for (int i=0; i<image1.rows; i++){
	for (int j=0; j<image1.cols; j++){
	  if(i < unfocus_width) {
		std::cout << "Original value at (i, j): " << alpha_matrix.at<float>(i,j) << std::endl;
		alpha_matrix.at<float>(i, j) = upper_increment;
		std::cout << "Replaced value at (i, j): " << upper_increment << std::endl;
		upper_increment = upper_increment + 1.0/unfocus_intensity;
	  }
	  else if(i >= alpha_matrix.rows - unfocus_width){
		alpha_matrix.at<float>(i, j) = lower_increment;
		lower_increment = lower_increment + 1.0/unfocus_intensity;
	  }
	}
  }

  cv::imshow("Alpha matrix", alpha_matrix);
  average_filter(image1, image2);

  image2.copyTo(imageTop);
  cv::namedWindow("addweighted", 1);

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

  cv::waitKey(0);
  return 0;
}
