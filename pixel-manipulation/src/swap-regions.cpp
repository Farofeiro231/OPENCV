#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <ostream>
#include <sys/types.h>

void swap_image(cv::Mat &src_image, cv::Mat &out_image)
{
  int rows = src_image.rows;
  int cols = src_image.cols;
  cv::Mat diagonal = cv::Mat::eye(rows, cols, CV_32F);
  cv::Mat anti_diagonal;
  cv::flip(src_image, out_image, -1);
  cv::Mat top_left = src_image({0, cols/2}, {0, rows/2});
  cv::Mat top_right = src_image({0, cols/2}, {rows/2+1, rows-1});
  cv::Mat bottom_left = src_image({cols/2+1, cols-1}, {0, rows/2});
  cv::Mat bottom_right = src_image({cols/2+1, cols-1}, {rows/2+1, rows-1});
  // Swapped image's top left
  //out_image(cv::Range(0, cols/2), cv::Range(0, rows/2)) = bottom_right.clone(); 
  cv::imshow("Window", out_image);
  cv::waitKey();
  // Swapped image's bottom right
  out_image({cols/2+1, cols-1}, {rows/2+1, rows-1}) = top_left; 
  // Swapped image's top right
  out_image({0, cols/2}, {rows/2+1, rows-1}) = bottom_left; 
  // Swapped image's bottom left
  out_image({cols/2+1, cols-1}, {0, rows/2}) = top_right; 

  cv::imshow("Window", out_image);
  cv::waitKey();
}


int main(int argc, char** argv){
  cv::Mat image;
  cv::Mat swapped_image;

  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if(!image.data){
	std::cerr << "It was not possible to open the imagem in question! Exiting..." << std::endl;
	return 0;
  }

  swapped_image = image;

  cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Window", image);
  cv::waitKey();

  swap_image(image, swapped_image);

  cv::imshow("Window", swapped_image);
  cv::waitKey();

  return 0;
}
