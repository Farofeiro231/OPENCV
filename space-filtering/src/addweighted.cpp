#include <algorithm>
#include <bits/stdint-uintn.h>
#include <cmath>
#include <cstdint>
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
  
  int upper_increment = 0;
  int lower_increment = 255;
  int unfocus_width = 100;
  int unfocus_intensity = 50;
  int offset = 0;
  cv::Mat alpha_matrix(image1.rows, image1.cols, CV_8UC1, cv::Scalar(255, 255, 255));//CV_8UC1, cv::Scalar(255,255,255));
  std::cout << "Image1.cols: " << image1.cols << std::endl;
  std::cout << "alpha_matrix.cols: " << alpha_matrix.cols << std::endl;
  int _stride = alpha_matrix.step;
  std::uint8_t *matrix_data = alpha_matrix.data;
  cv::imshow("Alpha matrix original", alpha_matrix);
  // This offset variable is used to compensate the bigger black region at
  // the bottom of the mask created if the user picks too small a intensity for
  // the given unfocus_widht.
  offset = std::max(image1.rows - unfocus_width, 0);
  for (int i=0; i<image1.rows; i++){
	uint8_t* p = alpha_matrix.ptr(i);
	// Add 1 step to the increments given the value of i
	// meets the correct conditions.
	if (i + offset < unfocus_width)
	  upper_increment = upper_increment + floor(255.0/unfocus_intensity);
	if(i >= (alpha_matrix.rows - unfocus_width))
	  lower_increment = lower_increment - floor(255.0/unfocus_intensity);
	for (int j=0; j<image1.cols; j++){
	  if(i < unfocus_width) {
		*p++ = std::min(upper_increment, 255);
		std::cout << "(i, j): " << i << ", " << j << std::endl;
	  }
	  else if(i >= (alpha_matrix.rows - unfocus_width)){
		*p++ = std::max(lower_increment, 0);
		std::cout << "(i, j): " << i << ", " << j << std::endl;
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
