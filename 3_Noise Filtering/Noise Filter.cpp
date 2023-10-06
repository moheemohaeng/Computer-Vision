#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <time.h>
#include <iostream>

using namespace cv;
using namespace std;

void Gaussian_noise(Mat &input, Mat &output, double std);
void Salt_and_Pepper_noise(Mat &input, double noise_ratio);
void filter(Mat &img, Mat &output_img, Mat &mask, int mask_rows, int mask_cols);
void median_filter(Mat &img, Mat &output_img);
void quickSort(float arr[], int size);

int main() {
	Mat source_img = imread("Lena.png");
	
	int h = source_img.rows;
	int w = source_img.cols;

	//Gaussian noise
	Mat Gaussian_image1(h, w, CV_8UC3, Scalar(0));
	Gaussian_noise(source_img, Gaussian_image1, 30);
	Mat Gaussian_image2(h, w, CV_8UC3, Scalar(0));
	Gaussian_noise(source_img, Gaussian_image2, 60);
	Mat Gaussian_image3(h, w, CV_8UC3, Scalar(0));
	Gaussian_noise(source_img, Gaussian_image3, 100);

	//salt pepper noise
	Mat S_P_image1(h, w, CV_8UC3, Scalar(0));
	S_P_image1 = source_img.clone();
	Mat S_P_image2(h, w, CV_8UC3, Scalar(0));
	S_P_image2 = source_img.clone();
	Mat S_P_image3(h, w, CV_8UC3, Scalar(0));
	S_P_image3 = source_img.clone();
	Salt_and_Pepper_noise(S_P_image1, 0.05);
	Salt_and_Pepper_noise(S_P_image2, 0.1);
	Salt_and_Pepper_noise(S_P_image3, 0.25);

	//3X3 smoothing
	float aver_data[] = {	//3X3 moving average
		1 / 9.f,1 / 9.f,1 / 9.f,
		1 / 9.f,1 / 9.f,1 / 9.f,
		1 / 9.f,1 / 9.f,1 / 9.f
	};

	Mat aver_mask(3, 3, CV_32F, aver_data);

	Mat aver_outimage1_1(h, w, CV_8UC3, Scalar(0));
	filter(Gaussian_image1, aver_outimage1_1, aver_mask, 3, 3);
	Mat aver_outimage2_1(h, w, CV_8UC3, Scalar(0));
	filter(Gaussian_image1, aver_outimage2_1, aver_mask, 3, 3);
	Mat aver_outimage3_1(h, w, CV_8UC3, Scalar(0));
	filter(Gaussian_image1, aver_outimage3_1, aver_mask, 3, 3);

	//5X5 smoothing
	float aver_data2[] = {		//5X5 moving average
		1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
		1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
		1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
		1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
		1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f
	};
	Mat aver_mask2(5, 5, CV_32F, aver_data2);

	Mat aver_outimage1_2(h, w, CV_8UC3, Scalar(0));
	filter(Gaussian_image1, aver_outimage1_2, aver_mask2, 5, 5);
	Mat aver_outimage2_2(h, w, CV_8UC3, Scalar(0));
	filter(Gaussian_image2, aver_outimage2_2, aver_mask2, 5, 5);
	Mat aver_outimage3_2(h, w, CV_8UC3, Scalar(0));
	filter(Gaussian_image3, aver_outimage3_2, aver_mask2, 5, 5);

	// median_filter 3X3
	Mat median_outimage1(h, w, CV_8UC3, Scalar(0));
	median_filter(S_P_image1, median_outimage1);
	Mat median_outimage2(h, w, CV_8UC3, Scalar(0));
	median_filter(S_P_image2, median_outimage2);
	Mat median_outimage3(h, w, CV_8UC3, Scalar(0));
	median_filter(S_P_image3, median_outimage3);


	imshow("Original", source_img);
	imshow("GaussianNoise1", Gaussian_image1);
	imshow("GaussianNoise2", Gaussian_image2);
	imshow("GaussianNoise3", Gaussian_image3);

	imshow("1_1Smoothing3X3", aver_outimage1_1);
	imshow("2_1Smoothing3X3", aver_outimage2_1);
	imshow("3_1Smoothing3X3", aver_outimage3_1);

	imshow("1_2Smoothing5X5", aver_outimage1_2);
	imshow("2_2Smoothing5X5", aver_outimage2_2);
	imshow("3_2Smoothing5X5", aver_outimage3_2);

	imshow("SaltPepperNoise1", S_P_image1);
	imshow("SaltPepperNoise2", S_P_image2);
	imshow("SaltPepperNoise3", S_P_image3);

	imshow("MedianFilter1", median_outimage1);
	imshow("MedianFilter2", median_outimage2);
	imshow("MedianFilter3", median_outimage3);
	waitKey(0);

	return 0;
}
void Gaussian_noise(Mat &input, Mat &output, double std) {
	Mat noise_image(input.rows, input.cols, CV_8UC3, Scalar(0));
	double average = 0.0;
	randn(noise_image, Scalar::all(average), Scalar::all(std));
	input.convertTo(output, CV_8UC3);
	addWeighted(output, 1.0, noise_image, 1.0, 0.0, output);
	output.convertTo(output, input.type());
}
void Salt_and_Pepper_noise(Mat &input, double noise_ratio) {
	int rows = input.rows;
	int cols = input.cols;
	int ch = input.channels();
	int num_of_noise_pixels = (int)((double)(rows*cols*ch)*noise_ratio);
	
	if (input.channels() == 3) {	//컬러이미지
		for (int i = 0; i < num_of_noise_pixels; i++) {
			int r = rand() % rows;
			int c = rand() % cols;
			int _ch = rand() % ch;
			uchar* pixel = input.ptr<uchar>(r) + (c*ch) + _ch;
			*pixel = (rand() % 2 == 1) ? 255 : 0;
		}
	}
	else if (input.channels() == 1) {	//흑백이미지
		for (int i = 0; i < num_of_noise_pixels; i++) {
			int r = rand() % rows;
			int c = rand() % cols;
			uchar* pixel = input.ptr<uchar>(r) + (c*ch);
			*pixel = (rand() % 2 == 1) ? 255 : 0;
		}
	}
}

void filter(Mat &img, Mat &output_img, Mat &mask, int mask_rows, int mask_cols) {
	Mat copy_plate(img.rows + mask_rows / 2 * 2, img.cols + mask_cols / 2 * 2, CV_32F, Scalar(0));//복사, 계산용 

	uchar* copy_data = copy_plate.data;
	uchar* img_data = img.data;
	uchar* output_data = output_img.data;
	uchar* mask_data = mask.data;

	//zero padding제외 가운데 원본사진복사
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			if (img.channels() == 3) {
				copy_data[(row + mask_rows / 2)*(copy_plate.cols) * 3 + (col + mask_cols / 2) * 3] = img_data[row * img.cols * 3 + col * 3];
				copy_data[(row + mask_rows / 2)*(copy_plate.cols) * 3 + (col + mask_cols / 2) * 3 + 1] = img_data[row * img.cols * 3 + col * 3 + 1];
				copy_data[(row + mask_rows / 2)*(copy_plate.cols) * 3 + (col + mask_cols / 2) * 3 + 2] = img_data[row * img.cols * 3 + col * 3 + 2];
			}
			else if (img.channels() == 1) {
				copy_data[(row + mask_rows / 2)*(copy_plate.cols) + (col + mask_cols / 2)] = img_data[row * img.cols + col];
			}
		}
	}

	//filter 적용
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			if (img.channels() == 3) {	//컬러이미지 일때
				float sum_b = 0;
				float sum_g = 0;
				float sum_r = 0;
				for (int mask_r = 0; mask_r < mask.rows; mask_r++) {
					for (int mask_c = 0; mask_c < mask.cols; mask_c++) {
						sum_b += mask.at<float>(mask_r, mask_c) * copy_data[(row + mask_r)*(copy_plate.cols) * 3 + (col + mask_c) * 3];
						sum_g += mask.at<float>(mask_r, mask_c) * copy_data[(row + mask_r)*(copy_plate.cols) * 3 + (col + mask_c) * 3 + 1];
						sum_r += mask.at<float>(mask_r, mask_c) * copy_data[(row + mask_r)*(copy_plate.cols) * 3 + (col + mask_c) * 3 + 2];
					}
				}

				if (sum_b > 255) sum_b = 255;
				else if (sum_b < 0) sum_b = 0;
				if (sum_g > 255) sum_g = 255;
				else if (sum_g < 0) sum_g = 0;
				if (sum_r > 255) sum_r = 255;
				else if (sum_r < 0) sum_r = 0;

				output_data[row * img.cols * 3 + col * 3] = sum_b;
				output_data[row * img.cols * 3 + col * 3 + 1] = sum_g;
				output_data[row * img.cols * 3 + col * 3 + 2] = sum_r;
			}
			else if (img.channels() == 1) {	//흑백이미지 일때
				float sum = 0;
				for (int mask_r = 0; mask_r < mask.rows; mask_r++) {
					for (int mask_c = 0; mask_c < mask.cols; mask_c++) {
						sum += mask.at<float>(mask_r, mask_c) * copy_data[(row + mask_r)*(copy_plate.cols) + (col + mask_c)];
					}
				}
				if (sum > 255) {
					sum = 255;
				}
				else if (sum < 0) {
					sum = 0;
				}
				output_data[row * img.cols + col] = sum;

			}
		}
	}
}
void median_filter(Mat &img, Mat &output_img) {
	Mat copy_plate(img.rows + 2, img.cols + 2, CV_32F, Scalar(0));//복사, 계산용 

	uchar* copy_data = copy_plate.data;
	uchar* img_data = img.data;
	uchar* output_data = output_img.data;


	//zero padding제외 가운데 원본사진복사
	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			if (img.channels() == 3) {
				copy_data[(row + 3 / 2)*(copy_plate.cols) * 3 + (col + 3 / 2) * 3] = img_data[row * img.cols * 3 + col * 3];
				copy_data[(row + 3 / 2)*(copy_plate.cols) * 3 + (col + 3 / 2) * 3 + 1] = img_data[row * img.cols * 3 + col * 3 + 1];
				copy_data[(row + 3 / 2)*(copy_plate.cols) * 3 + (col + 3 / 2) * 3 + 2] = img_data[row * img.cols * 3 + col * 3 + 2];
			}
			else if (img.channels() == 1) {
				copy_data[(row + 3 / 2)*(copy_plate.cols) + (col + 3 / 2)] = img_data[row * img.cols + col];
			}
		}
	}

	for (int row = 0; row < img.rows; row++)
	{
		for (int col = 0; col < img.cols; col++)
		{
			if (img.channels() == 3) {	//컬러이미지 일때

				float arr_b[9];
				float arr_g[9];
				float arr_r[9];
				for (int r = 0; r < 3; r++) {
					for (int c = 0; c < 3; c++) {
						arr_b[3*r + c] = copy_data[(row + r)*(copy_plate.cols) * 3 + (col + c) * 3];
						arr_g[3*r + c] = copy_data[(row + r)*(copy_plate.cols) * 3 + (col + c) * 3 + 1];
						arr_r[3*r + c] = copy_data[(row + r)*(copy_plate.cols) * 3 + (col + c) * 3 + 2];
					}
				}

				quickSort(arr_b, 9);
				quickSort(arr_g, 9);
				quickSort(arr_r, 9);
				output_data[row * img.cols * 3 + col * 3] = arr_b[4];
				output_data[row * img.cols * 3 + col * 3 + 1] = arr_g[4];
				output_data[row * img.cols * 3 + col * 3 + 2] = arr_r[4];
			}
			else if (img.channels() == 1) {	//흑백이미지 일때

				float arr[9];
				for (int r = 0; r < 3; r++) {
					for (int c = 0; c < 3; c++) {
						arr[3*r + c] = copy_data[(row + r)*(copy_plate.cols) + (col + c)];
					}
				}
				
				quickSort(arr, 9);
				output_data[row * img.cols + col] = arr[4];

			}
		}
	}
}

void quickSort(float arr[], int size) {
	int pivot = arr[0];
	int cursor = 0;

	for (int i = 1; i < size; i++) {
		if (pivot > arr[i]) {
			cursor++;
			swap(arr[cursor], arr[i]);
		}
	}
	swap(arr[0], arr[cursor]);
	if (cursor > 0) {
		quickSort(arr, cursor);
	}
	if (cursor + 1 < size - 1) {
		quickSort(arr + cursor + 1, size - cursor - 1);
	}
}