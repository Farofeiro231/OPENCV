#include <iostream>
#include <opencv2/core/hal/interface.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

void eliminate_edge_objects(cv::Mat &image)
{
  cv::Point p;
  int rows = image.rows;
  int cols = image.cols;

  // I'm using j as the first index because x represent the vertical axis (orientes downwards)
  // in the OpenCV referential.
  for(int j=0; j<rows; j=j+rows-1){
	for(int i=0; i<cols; i+=1){
	  if(image.at<uchar>(i, j) == 255){
		p.x = i;
		p.y = j;
		cv:floodFill(image, p, 0);
	  }
	}
  }

}

int main(int argc, char** argv){
  cv::Mat image, realce;
  int width, height;
  int nobjects;

  cv::Point p;
  image = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);

  if(!image.data){
    std::cerr << "The image didn't load correctly. Exiting...\n";
    return(-1);
  }

  width=image.cols;
  height=image.rows;
  std::cout << "The image dimensions are: " << width << "x" << height << std::endl;

  p.x=0;
  p.y=0;

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
