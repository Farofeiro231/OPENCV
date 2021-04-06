#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/base.hpp>
#include "opencv2/opencv.hpp"
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>
#include <cstdlib>

using namespace std;
using namespace cv;

#define STEP 5
#define JITTER 3
#define RAIO 3

int top_slider = 10;
int top_slider_max = 200;

char TrackbarName[50];

Mat image, border, art;

cv::Mat dotilism(cv::Mat& image){
  vector<int> yrange;
  vector<int> xrange;

  Mat frame, points;

  int width, height, gray;
  int x, y;

  width=image.size().width;
  height=image.size().height;

  xrange.resize(height/STEP);
  yrange.resize(width/STEP);

  iota(xrange.begin(), xrange.end(), 0);
  iota(yrange.begin(), yrange.end(), 0);

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
	  x = i+rand()%(2*JITTER)-JITTER+1;
	  y = j+rand()%(2*JITTER)-JITTER+1;
	  gray = image.at<uchar>(x,y);
	  circle(points,
			 cv::Point(y,x),
			 RAIO,
			 CV_RGB(gray,gray,gray),
			 -1,
			 LINE_AA);
	}
  }

  imwrite("pontos.jpg", points);
  return points;
}


void on_trackbar_canny(int, void*){
  Canny(image, border, top_slider, 3 * top_slider);
  // cv::normalize(border, border, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
  std::cout << "Border: " << border << std::endl;
  art = dotilism(image);
  // imshow("canny", image.mul(border));
  imshow("canny", art + border);
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

  waitKey();
  imwrite("cannyborders.png", border);
  return 0;
}
