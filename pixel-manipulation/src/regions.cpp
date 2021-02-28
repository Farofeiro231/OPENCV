#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/opencv.hpp>
#include <ostream>
#include <sys/types.h>
#include <vector>

void get_coordinates(std::vector<int> &point1, std::vector<int> &point2)
{
  // This function needs some streamlining. A lisp macro would do the job. 
  int temp;
  const char* points[] = {"P(x)", "P(y)"};
  std::cout << "Please provide the requested coordinate" << std::endl;
  for (int i=0; i<2; i++){
	std::cout << "P( " << (i==0?"X):":"Y):") << std::endl;
	std::cin >> temp;
	point1.push_back(temp);
  }
  for (int i=0; i<2; i++){
	std::cout << "P( " << (i==0?"X):":"Y):") << std::endl;
	std::cin >> temp;
	point2.push_back(temp);
  }
}

void negate_image(cv::Mat &src_image, cv::Mat &out_image, const std::vector<int> point1, const std::vector<int> point2)
{

  // This vector is used to "negate" the colors in the original image.
  cv::Vec3b white;
  white[0] = 255;
  white[1] = 255;
  white[2] = 255;
  
  for(int i=point1[0];i<point1[1];i++){
    for(int j=point2[1];i<point2[1];i++){
	  out_image.at<cv::Vec3b>(i,j) = white - src_image.at<cv::Vec3b>(i,j);
    }
  }

}

int main(int argc, char** argv){
  cv::Mat image;
  cv::Mat negative_image;
  cv::Vec3b val;
  std::vector<int> p1(2,0);
  std::vector<int> p2(2,0);
  uint temp;

  image = cv::imread(argv[0], cv::IMREAD_GRAYSCALE);
  if(!image.data){
	std::cerr << "It was not possible to open the imagem in question! Exiting..." << std::endl;
	return 0;
  }

  negative_image = image;

  // Coordinates acquisition for the negative region.
  std::cout << "Let's set P1 and P2 as delimiters to the area of the image where the negative effect will be applied." << std::endl;
  get_coordinates(p1, p2);

  cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);

  negate_image(image, negative_image, p1, p2);

  cv::imshow("Negative", negative_image);
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
