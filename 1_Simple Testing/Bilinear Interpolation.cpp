#include <opencv2/opencv.hpp>


using namespace cv;
using namespace std;

#include <iostream>
#include <math.h>
void Bilinear_Interpolation(Mat &image1, Mat &image2, double x_rate, double y_rate) {
	for (int y = 0; y < image2.rows; y++) {
		for (int x = 0; x < image2.cols; x++) {
			int px = (int)(x / x_rate);
			int py = (int)(y / y_rate);

			double fx1 = (double)x / (double)x_rate - (double)px;
			double fx2 = 1 - fx1;
			double fy1 = (double)y / (double)y_rate - (double)py;
			double fy2 = 1 - fy1;

			double w1 = fx2 * fy2;
			double w2 = fx1 * fy2;
			double w3 = fx2 * fy1;
			double w4 = fx1 * fy1;

			uchar P1 = image1.at<uchar>(py, px);
			uchar P2 = image1.at<uchar>(py, px + 1);
			uchar P3 = image1.at<uchar>(py + 1, px);
			uchar P4 = image1.at<uchar>(py + 1, px + 1);
			image2.at<uchar>(y, x) = w1 * P1 + w2 * P2 + w3 * P3 + w4 * P4;


		}
	}
}
int main() {
	Mat img_in;
	double x_size = 0;
	double y_size = 0;
	cout << "x_size(128~1024) : ";
	cin >> x_size;
	cout << "y_size(128~1024) : ";
	cin >> y_size;
	//image 읽고 gray로 바꾸기
	img_in = imread("Lena_256x256.png");
	cvtColor(img_in, img_in, cv::COLOR_RGB2GRAY);
	imshow("Original img", img_in);

	double x_scale = (double)x_size/(double)255;
	double y_scale = (double)y_size/(double)255;
	
	int h = img_in.rows; //256
	int w = img_in.cols; //256

	int imgcstate =  CV_8UC1 ; //흑백
	Mat outimage((h-1)*y_scale, (w-1)*x_scale, imgcstate, Scalar(0));

	Bilinear_Interpolation(img_in, outimage, x_scale, y_scale);

	imshow("Output Image", outimage);
	waitKey(0);
	return 0;
}

