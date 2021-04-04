//------------------------------------------------------------------------------------------//
// Author: Emanoel de Sousa Costa.															//
// This code takes a colored image as the input, calculates its histogram and		//
// performs an histogram equalization on the original image. The output is shown on screen. //
//------------------------------------------------------------------------------------------//

#include <cmath>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <ostream>
#include <vector>

void equalizeHistogram(cv::Mat &src, std::vector<cv::Mat> &planes, cv::Mat &histR, cv::Mat &histG, cv::Mat &histB, int nbins, const float *histrange)
{
  
  const int levels = 255;
  cv::split (src, planes);

  // In th OpenCV standard, the blue is the first channel, followed by the green one; the last one
  // is the red channel.

  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histB, 1,
			   &nbins, &histrange,
			   true, false);
  cv::calcHist(&planes[1], 1, 0, cv::Mat(), histG, 1,
			   &nbins, &histrange,
			   true, false);
  cv::calcHist(&planes[2], 1, 0, cv::Mat(), histR, 1,
			   &nbins, &histrange,
			   true, false);

  auto tipo = histR.type();
  std::vector<cv::Mat> accumulated_histogram(3, cv::Mat(histR.rows, histR.cols, tipo));
  std::vector<cv::Mat> equalization_function(3, cv::Mat(histR.rows, histR.cols, tipo));

  // Here I calculate the accumulated histogram for the red (index = 0), green (index = 1) e blue (index = 2) colors.
  // I calculated them separetely and then perform the transformations on each individual color-channel. After that,
  // I merge all of the color channels into the original image.
  
  // Here I calculate  the accumulated histogram for each channel.

  for(int i=0; i<nbins; i++){
  	if(i > 0){
  	  accumulated_histogram[0].at<int>(i) = histB.at<float>(i) + accumulated_histogram[0].at<int>(i-1);
  	  accumulated_histogram[1].at<int>(i) = histG.at<float>(i) + accumulated_histogram[1].at<int>(i-1);
  	  accumulated_histogram[2].at<int>(i) = histR.at<float>(i) + accumulated_histogram[2].at<int>(i-1);
  	}
  	else{
  	  accumulated_histogram[0].at<int>(i) = histB.at<float>(i);
  	  accumulated_histogram[1].at<int>(i) = histG.at<float>(i);
  	  accumulated_histogram[2].at<int>(i) = histB.at<float>(i);
  	}
  }

  // Using the accumulated histogram, I calculate the values of the equalization function for each one of the bins
  // of the histogram.

  for (int i=0; i<nbins; i++){
  	equalization_function[0].at<int>(i) = levels*(accumulated_histogram[0].at<int>(i))/(accumulated_histogram[0].at<int>(nbins-1));
  	equalization_function[1].at<int>(i) = levels*(accumulated_histogram[1].at<int>(i))/(accumulated_histogram[1].at<int>(nbins-1));
  	equalization_function[2].at<int>(i) = levels*(accumulated_histogram[2].at<int>(i))/(accumulated_histogram[2].at<int>(nbins-1));
  }

  for(int i=0; i<src.rows; i++){
  	for(int j=0; j<src.cols; j++){
  	  planes[0].at<uchar>(i, j) = equalization_function[0].at<int>(std::floor(planes[0].at<uchar>(i, j)/4));
  	  planes[1].at<uchar>(i, j) = equalization_function[1].at<int>(std::floor(planes[1].at<uchar>(i, j)/4));
  	  planes[2].at<uchar>(i, j) = equalization_function[2].at<int>(std::floor(planes[2].at<uchar>(i, j)/4));
  	}
  }

  // Finally, once the planes are all equalized, I merge them back into the original image.
  cv::merge(planes, src);
}

int main(int argc, char** argv)
{
  cv::Mat image;
  int width, height;
  cv::VideoCapture cap;
  std::vector<cv::Mat> planes;
  cv::Mat histR, histG, histB;
  int nbins = 64;
  float range[] = {0, 255};
  const float *histrange = { range };
  bool uniform = true;
  bool acummulate = false;
  int key;

  cap.open(0);

  if(!cap.isOpened()){
	std::cerr << "There are no available cameras. Exiting...";
	return -1;
  }

  cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
  width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

  std::cout << "largura = " << width << std::endl;
  std::cout << "altura  = " << height << std::endl;

  // The width of the histogram needs to match the number of bins

  int histw = nbins;
  int histh = nbins/2;
  cv::Mat histImgR(histh, histw, CV_8UC3, cv::Scalar(0,0,0));
  cv::Mat histImgG(histh, histw, CV_8UC3, cv::Scalar(0,0,0));
  cv::Mat histImgB(histh, histw, CV_8UC3, cv::Scalar(0,0,0));

  while(1){
    cap >> image;

    cv::imshow("Original image", image);

	equalizeHistogram(image, planes, histR, histG, histB, nbins, histrange);

    cv::normalize(histR, histR, 0, histImgR.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histG, histG, 0, histImgG.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histB, histB, 0, histImgB.rows, cv::NORM_MINMAX, -1, cv::Mat());

    histImgR.setTo(cv::Scalar(0));
    histImgG.setTo(cv::Scalar(0));
    histImgB.setTo(cv::Scalar(0));

	for(int i = 0; i < nbins; i++){
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
	histImgR.copyTo(image(cv::Rect(0, 0       ,nbins, histh)));
    histImgG.copyTo(image(cv::Rect(0, histh   ,nbins, histh)));
    histImgB.copyTo(image(cv::Rect(0, 2*histh ,nbins, histh)));
    cv::imshow("Equalized image", image);
    key = cv::waitKey(30);
	
    if(key == 27) break;
  }
  return 0;
}
