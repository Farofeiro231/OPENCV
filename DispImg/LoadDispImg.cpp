#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char **argv)
{
	if (argc != 2)
	{
	 cout << "Usage: display_Image ImageToLoadAndDisplay" << endl;
	 return -1;
	}

	Mat image;
	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);	//Lê o arquivo

	if (!image.data)				//Verifica se a entrada é valida.
	{
	 cout << "Could not open or find the image" << std::endl;
	 return -1;
	}

	namedWindow( "Display window", WINDOW_AUTOSIZE);	//Cria uma janela para mostrar a imagem.
	imshow ( "Display window", image);			//Mostra a imagem dentro

	waitKey(0);
	return 0;
}
