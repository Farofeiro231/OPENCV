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

int height_slider = 54;
int height_slider_max = 100;

int width_slider = 0;
int width_slider_max = 100;

int intensity_slider = 100;
int intensity_slider_max = 500;

cv::Mat image1, image2, blended, mask;
cv::Mat imageTop, imageBottom;

char TrackbarName[50];

void modify_mask(cv::Mat &mask)
{
  float upper_step = 0;
  float lower_step = 0;
  int focus_width = width_slider*(mask.cols)/100;
  int focus_height = height_slider*(mask.rows)/100;
  float focus_intensity = 1.0*intensity_slider/100;
  cv::Mat(mask.rows, mask.cols, CV_8UC3, cv::Scalar(255, 255, 255)).copyTo(mask);
  std::vector<float> upper_val = {0, 0, 0};
  std::vector<float> lower_val = {1, 1, 1};

  // I used the matrix_data approach because I wanted to see if there was a noticeable difference
  // in speed by using that (in contrast with the at<> method).

  upper_step =  (1.0/(focus_height - focus_width/2.0)) * focus_intensity;
  lower_step =  (1.0/(mask.rows - (focus_height + focus_width/2.0))) * focus_intensity;

  // The focus region has the following characterístics: it's centered around the focus_height and it has
  // the remaining of the image outside [focus_height - focus_width/2, focus_height + focus_width/2]
  // as a transition zone above and below it.

  int lower_boundary = std::max(focus_height - focus_width/2, 0);
  int upper_boundary = std::min(focus_height + focus_width/2, mask.rows);
  int k = 0;
  for (int i = 0; i < mask.rows; i++) {
	std::cout << "Value of focus_height - focus_width/2: " << std::max(focus_height - focus_width/2, 0) << std::endl;
	std::cout << "upper step: " << upper_step << std::endl;
	std::cout << "focus height: " << focus_height << std::endl;
	  // The loop actually goes 3 times through i = n, because the image
	  // has 3 color channels; hence, I need to reset the increment value each time,
	  // otherwise it will show stripes on screen due to the resetting of the uchar
	  // value inside the image.
	  if (i < lower_boundary) {
		upper_val[0] += upper_step;
		upper_val[1] += upper_step;
		upper_val[2] += upper_step;
	  }
	  if (i >= upper_boundary) {
		lower_val[0] -= lower_step;
		lower_val[1] -= lower_step;
		lower_val[2] -= lower_step;
	  }

	  if (i < focus_height)
		std::cout << "i, Upper value: " << i << ", " << upper_val[0] * 255 << std::endl;

	  for (int j=0; j<mask.cols; j++) {
		// if (i < std::max(focus_height - focus_width/2, 0)) {
		if (upper_val[0] <= 1) {
		  mask.at<cv::Vec3b>(i, j)[0] = upper_val[0] * 255;
		  mask.at<cv::Vec3b>(i, j)[1] = upper_val[1] * 255;
		  mask.at<cv::Vec3b>(i, j)[2] = upper_val[2] * 255;
		} else {
		  mask.at<cv::Vec3b>(i, j)[0] = 255;
		  mask.at<cv::Vec3b>(i, j)[1] = 255;
		  mask.at<cv::Vec3b>(i, j)[2] = 255;
		}
		// }
		// else if (i >= std::min(focus_height + focus_width/2, mask.rows)) {
		if (lower_val[0] >= 0) {
		  mask.at<cv::Vec3b>(i, j)[0] = lower_val[0] * 255;
		  mask.at<cv::Vec3b>(i, j)[1] = lower_val[1] * 255;
		  mask.at<cv::Vec3b>(i, j)[2] = lower_val[2] * 255;
		} else {
		  mask.at<cv::Vec3b>(i, j)[0] = 0;
		  mask.at<cv::Vec3b>(i, j)[1] = 0;
		  mask.at<cv::Vec3b>(i, j)[2] = 0;
		}
		// }
	  }
	}
}


void average_filter(cv::Mat &src, cv::Mat &destination_img)
{
  cv::Mat average_mask = cv::Mat::ones(5, 5, CV_32F) * 0.1;//0.04;
  cv::filter2D(src, destination_img, src.depth(), average_mask, cv::Point(1, 1), 0);
}


cv::Mat& modify_frame(cv::Mat &original_frame)
{
  static cv::Mat averaged_frame;
  static cv::Mat temp;
  static cv::Mat reverse_mask;
  
  reverse_mask = cv::Mat(mask.rows, mask.cols, CV_8UC3, cv::Scalar(255, 255, 255)) - mask;
  average_filter(original_frame, averaged_frame);
  imageBottom = original_frame.mul(mask*(1.0/255));//cv::multiply(mask, mask*(1.0/255), blended);

  // When using the cv::Mat::ones method only the first channel is initialized to 1,
  // therefore I need to initialize the other two myself. I used an ordinary initialization instead.


  imageTop = averaged_frame.mul(reverse_mask * (1.0 / 255));//cv::multiply(mask, mask*(1.0/255), blended);
  blended = imageTop + imageBottom;
  return blended;
}


void mask_control(int, void*)
{
  modify_mask(mask);
  cv::Mat new_mat = cv::Mat(mask.rows, mask.cols, CV_8UC3, cv::Scalar(255, 255, 255)) - mask;
  cv::Mat reverse_mask = cv::Mat(mask.rows, mask.cols, CV_8UC3, cv::Scalar(255, 255, 255)).mul((new_mat * (1.0 / 255)));
  cv::imshow("Mask", mask);
  cv::imshow("Mask teste", reverse_mask);// * (1.0/255.0));
}


void modify_video(cv::VideoCapture &original_video)
{
  int frame_width = original_video.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = original_video.get(cv::CAP_PROP_FRAME_HEIGHT);
  cv::Mat current_frame;
  cv::Mat modified_frame;

  cv::VideoWriter modified_video("modified.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30,
								 cv::Size(frame_width, frame_height));

  // Here we'll loop through the video frames until the video file has been read in its entirety.
  // Each frame will be modified according to the selected mask.
  
  while (1) {

  	original_video >> current_frame;

  	if (current_frame.empty()) {
  	  break;
  	}

	modified_frame = modify_frame(current_frame);
	modified_video.write(modified_frame);

  	// Display the current frame
  	imshow("Frame", modified_frame);
  }

  // Releasing the memory used by the VideoWriter object
  modified_video.release();
}

int main(int argvc, char** argv){
  // Creating a VideoCapture object to hold the video file.
  cv::VideoCapture video = cv::VideoCapture("./figures/olaf_480p.mp4");

  int frame_width = video.get(cv::CAP_PROP_FRAME_WIDTH);
  int frame_height = video.get(cv::CAP_PROP_FRAME_HEIGHT);

  if (!video.isOpened()) {
	std::cerr << "Couldn't open the video file. Exiting!" << std::endl;
	return -1;
  }

  image1 = cv::imread("./figures/blend1.jpg");
  /* mask = cv::Mat(image1.rows, image1.cols, CV_8UC3, cv::Scalar(255, 255, 255));//CV_8UC1, cv::Scalar(255,255,255)); */
  mask = cv::Mat(frame_height, frame_width, CV_8UC3, cv::Scalar(255, 255, 255));//CV_8UC1, cv::Scalar(255,255,255));

  // Creation of the averaging mask and its application upon
  // the original image.
  
  /* average_filter(image1, image2); */

  /* image2.copyTo(imageTop); */
  cv::namedWindow("Mask", 1);

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

  // Waits forever for a keypress; when it happens, it either exits the program or writes the new
  // video file.
  char c = (char) cv::waitKey(0);
  if (c == 27) {
	video.release();
	return 0;
  } else if (c == 32) {
	modify_video(video);
	video.release();
	return 0;
  }
}
