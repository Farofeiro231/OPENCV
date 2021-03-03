//------------------------------------------------------------------------------------------//
// Author: Emanoel de Sousa Costa.															//
// This code takes a black-and-white image as the input, calculates its histogram and		//
// performs an histogram equalization on the original image. The output is shown on screen. //
//------------------------------------------------------------------------------------------//

#include <cmath>
#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/opencv.hpp>
#include <ostream>
#include <vector>

void equalizeHistogram(cv::Mat &src, std::vector<cv::Mat> &planes, cv::Mat &histR, cv::Mat &histG, cv::Mat &histB, int nbins, const float *histrange)
{
  std::cout << "Entrei na equalize" << std::endl;
  std::cout << src.rows << std::endl;
  std::cout << src.cols << std::endl;
  std::cout << nbins << std::endl;
  
  const int levels = 255;
  std::vector<cv::Mat> acummulated_histogram;
  std::vector<cv::Mat> equalizaiton_function;
  cv::split (src, planes);
  cv::calcHist(&planes[0], 1, 0, cv::Mat(), histR, 1,
			   &nbins, &histrange,
			   true, false);
  cv::calcHist(&planes[1], 1, 0, cv::Mat(), histG, 1,
			   &nbins, &histrange,
			   true, false);
  cv::calcHist(&planes[2], 1, 0, cv::Mat(), histB, 1,
			   &nbins, &histrange,
			   true, false);
  std::cout << "Tamanho de histR" << histR.size << std::endl;
  std::cout << "histR.at<float>(64) = " << histR.at<float>(63) << std::endl;
  std::cout << "Cheguei antes do for" << std::endl;

  // Here I calculate the acummulated histogram for the red (index = 0), green (index = 1) e blue (index = 2) colors.
  
  for(int i=0; i<nbins; i++){
	if(i > 0){
	  std::cout << "O valor de i é:" << std::endl;
	  acummulated_histogram[0].at<int>(i) = histR.at<int>(i) + acummulated_histogram[0].at<int>(i-1);
	  acummulated_histogram[1].at<int>(i) = histG.at<int>(i) + acummulated_histogram[1].at<int>(i-1);
	  acummulated_histogram[2].at<int>(i) = histB.at<int>(i) + acummulated_histogram[2].at<int>(i-1);
	}
	else{
	  acummulated_histogram[0].at<int>(i) = histR.at<int>(i);
	  acummulated_histogram[1].at<int>(i) = histG.at<int>(i);
	  acummulated_histogram[2].at<int>(i) = histB.at<int>(i);
	}
  }

  std::cout << "Passei do for" << std::endl;

  for (int i=0; i<nbins; i++){
	equalizaiton_function[0].at<int>(i) = levels*(acummulated_histogram[0].at<int>(i))/(acummulated_histogram[0].at<int>(nbins-1));
	equalizaiton_function[1].at<int>(i) = levels*(acummulated_histogram[1].at<int>(i))/(acummulated_histogram[1].at<int>(nbins-1));
	equalizaiton_function[2].at<int>(i) = levels*(acummulated_histogram[2].at<int>(i))/(acummulated_histogram[2].at<int>(nbins-1));
  }

  for(int i=0; i<src.rows; i++){
	for(int j=0; j<src.cols; j++){
	  src.at<int>(i, j) = equalizaiton_function[0].at<int>(std::floor(src.at<int>(i, j)/4));
	}
  }
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

	equalizeHistogram(image, planes, histR, histG, histB, nbins, histrange);

    cv::normalize(histR, histR, 0, histImgR.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histG, histG, 0, histImgG.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(histB, histB, 0, histImgB.rows, cv::NORM_MINMAX, -1, cv::Mat());

    histImgR.setTo(cv::Scalar(0));
    histImgG.setTo(cv::Scalar(0));
    histImgB.setTo(cv::Scalar(0));

    for(int i=0; i<nbins; i++){
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
    cv::imshow("image", image);
    key = cv::waitKey(30);
    if(key == 27) break;
  }
  return 0;
}
