#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main() {
	Mat img = imread("겨울이.jpg");
	printf(" \t(R, G, B) data : Mat::data \n");
	uchar* img_data = img.data;

	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			uchar b = img_data[row * img.cols * 3 + col * 3];
			uchar g = img_data[row * img.cols * 3 + col * 3 + 1];
			uchar r = img_data[row * img.cols * 3 + col * 3 + 2];
			uchar y = 0.299*r + 0.587*g + 0.114*b;
			uchar u = (b - y)*0.564 + 128; //cb
			uchar v = (r - y)*0.713 + 128; //cr
			printf("\t (%d, %d, %d)", y, u, v);
			img_data[row * img.cols * 3 + col * 3] = y;
			img_data[row * img.cols * 3 + col * 3 + 1] = v;
			img_data[row * img.cols * 3 + col * 3 + 2] = u;
		}
		cout << "\n" << endl;
	}


	//cv2.COLOR_BGR2YUV: BGR 색상 이미지를 YUV 색상 이미지로 변환
	Mat changeimg = img;
	cvtColor(img, changeimg, COLOR_BGR2YCrCb);
	uchar* changeimg_data = changeimg.data;
	for (int row = 0; row < changeimg.rows; row++)
	{
		for (int col = 0; col < changeimg.cols; col++)
		{
			//opencv에서는 YCbCr순서가 아닌 YCrCb순서로 인식한다.
			uchar y = changeimg_data[row * changeimg.cols * 3 + col * 3];
			uchar v = changeimg_data[row * changeimg.cols * 3 + col * 3 + 1]; //cr
			uchar u = changeimg_data[row * changeimg.cols * 3 + col * 3 + 2]; //cb

			printf("\t (%d, %d, %d)", y, u, v);
		}
		cout << "\n" << endl;
	}

	return 0;
}
