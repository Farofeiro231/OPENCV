#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include "opencv2/opencv.hpp"
#include <fstream>
#include <iomanip>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace cv;

#define STEP 5
#define EDGE_STEP 2
#define JITTER 3
#define EDGE_JITTER 1
#define RAIO 3
#define EDGE_RAIO 2

int top_slider = 10;
int top_slider_max = 200;

char TrackbarName[50];

Mat image, border, art;


cv::Mat edge_dotilism(cv::Mat& image){
  vector<int> yrange;
  vector<int> xrange;

  Mat frame, points;
  static Mat new_image, border_image;

  int width, height, gray;
  int x, y;

  width=image.size().width;
  height=image.size().height;

  xrange.resize(height/STEP);
  yrange.resize(width/STEP);

  iota(xrange.begin(), xrange.end(), 0);
  iota(yrange.begin(), yrange.end(), 0);

  Canny(image, border, top_slider, 3 * top_slider);
  cv::normalize(border, border, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  border_image = image.mul(border);
  
  for(uint i=0; i<xrange.size(); i++){
	xrange[i]= xrange[i]*STEP+STEP/2;
  }

  for(uint i=0; i<yrange.size(); i++){
	yrange[i]= yrange[i]*STEP+STEP/2;
  }

  points = Mat(height, width, CV_8U, Scalar(255));

  random_shuffle(xrange.begin(), xrange.end());

  for(auto i : xrange) {
	random_shuffle(yrange.begin(), yrange.end());
	for(auto j : yrange) {
	  x = i+rand()%(2*EDGE_JITTER)-EDGE_JITTER+1;
	  y = j+rand()%(2*EDGE_JITTER)-EDGE_JITTER+1;
	  gray = image.at<uchar>(x,y);
	  circle(points,
			 cv::Point(y,x),
			 RAIO,
			 CV_RGB(gray,gray,gray),
			 -1,
			 LINE_AA);
	}
  }

  for (int i = 0; i < border_image.rows; i++) {
	for (int j = 0; j < border_image.cols; j++) {
	  if (border_image.at<uchar>(i, j) != 0) {
		gray = border_image.at<uchar>(i, j);
		circle(points, cv::Point(j, i), EDGE_RAIO, CV_RGB(gray, gray, gray), -1, LINE_AA);
	  }
	}
  }

  imwrite("pontos.jpg", points);
  return points;
}


void on_trackbar_canny(int, void*){
  art = edge_dotilism(image);
  imshow("canny", art);
}


int main(int argc, char**argv){
  int width, height;

  image= imread(argv[1], IMREAD_GRAYSCALE);

  width=image.size().width;
  height=image.size().height;

  sprintf(TrackbarName, "Threshold inferior %d", top_slider_max);

  namedWindow("canny", 1);
  createTrackbar( TrackbarName, "canny",
                &top_slider,
                top_slider_max,
                on_trackbar_canny );

  on_trackbar_canny(top_slider, 0 );

  char c = (char) waitKey();
  while (1) {
	c = waitKey(0);
	if (c == 27)
	  break;
  }

  imwrite("cannyborders.png", border);
  return 0;
}
