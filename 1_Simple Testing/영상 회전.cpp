#include <opencv2/opencv.hpp>
#include <math.h>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;


void Geo_Rotate(Mat &image1, Mat &image2, int degree);
int main() {
	Mat img_in;
	int degree;
	cout << "input degree : ";
	cin >> degree;
	//image 읽고 gray로 바꾸기
	img_in = imread("Lena.png");
	cvtColor(img_in, img_in, cv::COLOR_RGB2GRAY);
	imshow("Original img", img_in);


	int h = img_in.rows;
	int w = img_in.cols;
	int imgcstate = CV_8UC1; //흑백


	Mat outimage(h, w, imgcstate, Scalar(0));
	Geo_Rotate(img_in, outimage, degree);

	imshow("Output Image", outimage);

	waitKey(0);
	return 0;
}

void Geo_Rotate(Mat &image1, Mat &image2, int degree) {
	double h = image2.rows;
	double w = image2.cols;

	int centerY = h / 2;
	int centerX = w / 2;
	double seta = -degree * 3.14 / 180.0;

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int newY = (x - centerX)*sin(seta) + (y - centerY)*cos(seta) + centerY;
			int newX = (x - centerX)*cos(seta) - (y - centerY)*sin(seta) + centerX;


			if ((newX < 0.0) || (newX >= w) || (newY < 0.0) || (newY >= h)) {
				image2.at<uchar>(y, x) = 0;
			}
			else {
				uchar data = image1.at<uchar>(newY, newX);
				image2.at<uchar>(y, x) = data;
			}
			
		}

		
	}
}