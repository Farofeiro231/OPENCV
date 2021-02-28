#include <bits/stdint-uintn.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/matx.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <ostream>
#include <sys/types.h>
#include <vector>

// This function needs some streamlining. A lisp macro would do the job. 
void get_coordinates(std::vector<int> &point1, std::vector<int> &point2)
{
  std::cout << "P1(X): " << point1[0] << std::endl;
  std::cout << "P1(Y): " << point1[1] << std::endl;
  int temp;
  const char* points[] = {"P(x)", "P(y)"};
  std::cout << "Please provide the requested coordinates for P1" << std::endl;
  for (int i=0; i<2; i++){
	std::cout << "P1(" << (i==0?"X):":"Y):") << std::endl;
	std::cin >> temp;
	point1[i] = temp;
  }
  std::cout << "P1(X): " << point1[0] << std::endl;
  std::cout << "P1(Y): " << point1[1] << std::endl;
  std::cout << "Please provide the requested coordinates for P2" << std::endl;
  for (int i=0; i<2; i++){
	std::cout << "P2(" << (i==0?"X):":"Y):") << std::endl;
	std::cin >> temp;
	point2[i] = temp;
  }
}

void negate_image(cv::Mat &src_image, cv::Mat &out_image, std::vector<int> &point1, std::vector<int> &point2)
{
  // This vector is used to "negate" the colors in the original image.
  uchar white = 255;

  for(int x=point1[0];x<point2[0];x++){
	for(int y=point1[1];y<point2[1];y++){
	  std::cout << "Coords: (" << x << "," << y << ")" << std::endl;
	  out_image.at<uchar>(x,y) = white - src_image.at<uchar>(x,y);
	}
  }
}

int main(int argc, char** argv){
  cv::Mat image;
  cv::Mat negative_image;
  cv::Vec3b val;
  std::vector<int> p1{20, 100};
  std::vector<int> p2{30, 150};

  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if(!image.data){
	std::cerr << "It was not possible to open the imagem in question! Exiting..." << std::endl;
	return 0;
  }

  negative_image = image;

  // Coordinates acquisition for the negative region.
  std::cout << "Let's set P1 and P2 as delimiters to the area of the image where the negative effect will be applied." << std::endl;
  get_coordinates(p1, p2);

  cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Window", image);
  cv::waitKey();

  negate_image(image, negative_image, p1, p2);

  cv::imshow("Window", negative_image);
  cv::waitKey();

  return 0;

  //  image = cv::imread("../figuras/bolhas.png",cv::IMREAD_COLOR);
  //
  //  val[0] = 0;   //B
  //  val[1] = 120;   //G
  //  val[2] = 255; //R
  //
  //  for(int i=200;i<210;i++){
  //    for(int j=10;j<200;j++){
  //	  image.at<cv::Vec3b>(i,j)=val;
  //    }
  //  }
  //
  //  cv::imshow("janela", image);
  //  cv::waitKey();
  //  return 0;
}
