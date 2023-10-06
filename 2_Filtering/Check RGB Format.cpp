#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main() {
	Mat img = imread("°Ü¿ïÀÌ.jpg");
	printf(" \t(R, G, B) data : Mat::data \n");
	uchar* img_data = img.data;

	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			uchar b = img_data[row * img.cols * 3 + col * 3];
			uchar g = img_data[row * img.cols * 3 + col * 3 + 1];
			uchar r = img_data[row * img.cols * 3 + col * 3 + 2];
			printf("\t (%d, %d, %d)", r, g, b);
		}
		cout << "\n" << endl;
	}

	return 0;
}
