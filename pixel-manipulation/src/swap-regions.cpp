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
  cv::Mat aux_matrix = cv::Mat::zeros(rows/2, cols/2, CV_8U);
  cv::Mat top_left = src_image({0, cols/2}, {0, rows/2});
  cv::Mat top_right = src_image({0, cols/2}, {rows/2, rows});
  cv::Mat bottom_left = src_image({cols/2, cols}, {0, rows/2});
  cv::Mat bottom_right = src_image({cols/2, cols}, {rows/2, rows});

  // Here I'm assigning to the top_let of the image the bottom right of the original one.
  // the CV_8U is th
  // e defined uchar type for OpenCV.
  out_image({0, cols/2}, {0, rows/2}) = aux_matrix + bottom_right; 
  // Now it's the top_right getting the bottom_left
  out_image({0, cols/2}, {rows/2, rows}) = aux_matrix + bottom_left; 
  // Now it's the bottom_left getting the top_right
  out_image({cols/2, cols}, {0, rows/2}) = aux_matrix + top_right; 
  // Now it's the bottom_right getting the top_left
  out_image({cols/2, cols}, {rows/2, rows}) = aux_matrix + top_left; 
}


int main(int argc, char** argv){
  cv::Mat image;

  // Reading the image and exiting the program if there's an error
  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
  if(!image.data){
	std::cerr << "It was not possible to open the imagem in question! Exiting..." << std::endl;
	return 0;
  }

  // I defined the swapped_image after reading the image because it needs to know
  // the dimensions of the original image; otherwise, the compiler will throw me an
  // error when I try to do operations on it.
  cv::Mat swapped_image = cv::Mat::zeros(image.rows, image.cols, CV_8U);

  // Plotting the original image simply for display.
  cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Window", image);
  cv::waitKey();

  // Swapping the image's region
  swap_image(image, swapped_image);

  // Displaying the swapped regions.
  cv::imshow("Window", swapped_image);
  cv::waitKey();

  return 0;
}
