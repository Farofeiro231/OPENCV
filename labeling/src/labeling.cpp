#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;

void eliminate_edge_objects(cv::Mat &image)
{
  cv::Point p;
  int rows = image.rows;
  int cols = image.cols;

  // I'm using j as the first index because x represent the vertical axis (orientes downwards)
  // in the OpenCV referential.

  // Eliminates the bubbles touching the top and bottom edges.
  for(int j=0; j<rows; j=j+rows-1){
	for(int i=0; i<cols; i+=1){
	  if(image.at<uchar>(i, j) == 255){
		p.x = j;
		p.y = i;
		cv::floodFill(image, p, 0);
	  }
	}
  }

  // Eliminates the bubbles touching the left and right edges.
  for(int j=0; j<rows; j+=1){
	for(int i=0; i<cols; i=i+cols-1){
	  if(image.at<uchar>(i, j) == 255){
		p.x = j;
		p.y = i;
		cv::floodFill(image, p, 0);
	  }
	}
  }

}

int main(int argc, char** argv){
  cv::Mat image, realce;
  cv::Mat contoured;
  int width, height;
  int nobjects;

  // Taken from the documentation. Contours is a list of all the image's contours, and hierarchy
  // tells me which contours have children (i.e.: have other contours inside of them).

  std::vector<std::vector<cv::Point>> contours;
  std::vector<cv::Vec4i> hierarchy;
  cv::Scalar scalar_red = {100, 255, 100};
  cv::Point p;

  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if(!image.data){
    std::cerr << "The image didn't load correctly. Exiting...\n";
    return(-1);
  }

  contoured = image;
  for (int i=0; i < contours.size(); i+=1){
	cv::findContours(contoured, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	cv::drawContours(contoured, contours, i, scalar_red, 2, cv::LINE_8, hierarchy, 0);
  }


  cv::imshow("Contoured image", contoured);

  width=image.cols;
  height=image.rows;
  std::cout << "The image dimensions are: " << width << "x" << height << std::endl;

  p.x=0;
  p.y=0;

  cv::imshow("Original image", image);
  eliminate_edge_objects(image);

  // busca objetos presentes
  nobjects=0;
  for(int i=0; i<height; i++){
    for(int j=0; j<width; j++){
      if(image.at<uchar>(i,j) == 255){
        // achou um objeto
        nobjects++;
        p.x=j;
        p.y=i;
  		// preenche o objeto com o contador
		cv::floodFill(image,p,nobjects);
      }
    }
  }
  std::cout << "a figura tem " << nobjects << " bolhas\n";
  cv::equalizeHist(image, realce);
  cv::imshow("image", image);
  cv::imshow("realce", realce);
  cv::imwrite("labeling.png", image);
  cv::waitKey();
  return 0;
}
