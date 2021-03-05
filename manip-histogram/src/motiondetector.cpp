//------------------------------------------------------------------------------------------//
// Author: Emanoel de Sousa Costa.															
// This is a motion detection program. It works based on the difference between the current
// image's histogram and the one from the previous image.
//------------------------------------------------------------------------------------------//

#include <cmath>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <ostream>
#include <vector>

cv::Mat histB, histG, histR;
cv::Mat histB_plus, histG_plus, histR_plus;


double histogram_diff(cv::Mat hist_t, cv::Mat hist_t_plus)
{
  cv::Mat hist_diff;
  double min_diff, max_diff; //min_diff is not meant to be used; I just didn't want to use a placeholder

  cv::absdiff(hist_t_plus, hist_t, hist_diff);
  cv::minMaxLoc(hist_diff, &min_diff, &max_diff);
  return max_diff;
}

bool detect_motion(cv::Mat &image_t, cv::Mat &image_t_plus, std::vector<cv::Mat> &planes, int nbins, const float *histrange)
{
  std::vector<double> hist_max(3, 0);
  std::vector<int> movement_threshold = {1200, 1200, 1200};
  int circle_radius = 20;
  cv::Scalar circle_color = {0, 0, 255};
  cv::Point red_circle_center(image_t_plus.cols - 50, image_t_plus.rows - 50);
  cv::Mat hist_diff;

  cv::split (image_t, planes);
  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histB, 1,
			   &nbins, &histrange,
			   true, false);

  cv::calcHist(&planes[1], 1, 0, cv::Mat(), histG, 1,
			   &nbins, &histrange,
			   true, false);

  cv::calcHist(&planes[2], 1, 0, cv::Mat(), histR, 1,
			   &nbins, &histrange,
			   true, false);

  cv::split (image_t_plus, planes);
  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histB_plus, 1,
			   &nbins, &histrange,
			   true, false);
	
  cv::calcHist(&planes[1], 1, 0, cv::Mat(), histG_plus, 1,
			   &nbins, &histrange,
			   true, false);

  cv::calcHist(&planes[2], 1, 0, cv::Mat(), histR_plus, 1,
			   &nbins, &histrange,
			   true, false);

  // Here I calculate the maximum and minimum histogram differences between
  // the t and t_plus images. It is done once per color-channel.

  hist_max[0] = histogram_diff(histB, histB_plus);
  hist_max[1] = histogram_diff(histG, histG_plus);
  hist_max[2] = histogram_diff(histR, histR_plus);
  
  // Here I draw the circle indicating that movement has been detected.
  // If any of the thresholds are met, the circle is drawn on screen

  if (hist_max[0] > movement_threshold[0] || hist_max[1] > movement_threshold[1] || hist_max[2] > movement_threshold[2]){
	cv::circle(image_t_plus, red_circle_center, circle_radius, circle_color, -1);
  }
  
  return true;
}


void draw_histograms(cv::Mat &src, cv::Mat &histImgR, cv::Mat &histImgG, cv::Mat &histImgB, const float *histrange)
{
  int histw = histImgR.cols;
  int histh = histw/2;
  std::vector<cv::Mat> planes;

  cv::split (src, planes);
  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histB, 1,
			   &histw, &histrange,
			   true, false);

  cv::split (src, planes);
  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histG, 1,
			   &histw, &histrange,
			   true, false);

  cv::split (src, planes);
  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histR, 1,
			   &histw, &histrange,
			   true, false);

  cv::normalize(histR, histR, 0, histImgR.rows, cv::NORM_MINMAX, -1, cv::Mat());
  cv::normalize(histG, histG, 0, histImgG.rows, cv::NORM_MINMAX, -1, cv::Mat());
  cv::normalize(histB, histB, 0, histImgB.rows, cv::NORM_MINMAX, -1, cv::Mat());

  histImgR.setTo(cv::Scalar(0));
  histImgG.setTo(cv::Scalar(0));
  histImgB.setTo(cv::Scalar(0));

  for(int i=0; i<histw; i++){
	cv::line(histImgR,
			 cv::Point(i, histh),
			 cv::Point(i, histh-cvRound(histR.at<float>(i))),
			 cv::Scalar(0, 0, 255), 1, 8, 0);
	cv::line(histImgG,
			 cv::Point(i, histh),
			 cv::Point(i, histh-cvRound(histG.at<float>(i))),
			 cv::Scalar(0, 255, 0), 1, 8, 0);
	cv::line(histImgB,
			 cv::Point(i, histh),
			 cv::Point(i, histh-cvRound(histB.at<float>(i))),
			 cv::Scalar(255, 0, 0), 1, 8, 0);
  }
  
  histImgR.copyTo(src(cv::Rect(0, 0       ,histw, histh)));
  histImgG.copyTo(src(cv::Rect(0, histh   ,histw, histh)));
  histImgB.copyTo(src(cv::Rect(0, 2*histh ,histw, histh)));
}


int main(int argc, char** argv)
{
  cv::Mat image_t;
  cv::Mat image_t_plus;
  int width, height;
  cv::VideoCapture cap;
  std::vector<cv::Mat> planes;
  cv::Mat hist_diff;
  int nbins = 64;
  int key;
  float range[] = {0, 255};
  const float *histrange = { range };
  double hist_min = 0;
  double hist_max = 0;
  bool uniform = true;
  bool accumulate = false;

  cap.open(0);

  if(!cap.isOpened()){
	std::cerr << "There are no available cameras. Exiting...";
	return -1;
  }

  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
  width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  // The width of the histogram needs to match the number of bins

  int histw = nbins;
  int histh = nbins/2;
  cv::Mat histImgR(histh, histw, CV_8UC3, cv::Scalar(0,0,0));
  cv::Mat histImgG(histh, histw, CV_8UC3, cv::Scalar(0,0,0));
  cv::Mat histImgB(histh, histw, CV_8UC3, cv::Scalar(0,0,0));

  while(1){
    cap >> image_t;
	cap >> image_t_plus;

	detect_motion(image_t, image_t_plus, planes, nbins, histrange);
	draw_histograms(image_t_plus, histImgR, histImgG, histImgB, histrange);

    cv::imshow("Image", image_t_plus);
    key = cv::waitKey(30);
    if(key == 27) break;
  }

  return 0;
}
