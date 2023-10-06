#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

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
void sharpening(Mat &img, Mat &output_img) {


	uchar* img_data = img.data;

	uchar* lap_data;
	uchar* output_data = output_img.data;

	float laplacian_data[] = {
		0, -1, 0,
		-1, 4, -1,
		0, -1, 0
	};
	Mat lapla_mask(3, 3, CV_32F, laplacian_data);

	if (img.channels() == 3) {
		Mat lapla_outimage(img.rows, img.cols, CV_8UC3, Scalar(0));
		filter(img, lapla_outimage, lapla_mask, 3, 3);
		lap_data = lapla_outimage.data;
		for (int row = 0; row < img.rows; row++) {
			for (int col = 0; col < img.cols; col++) {
				float sum_b = 0;
				float sum_g = 0;
				float sum_r = 0;
				sum_b = img_data[row * img.cols * 3 + col * 3] + lap_data[row * lapla_outimage.cols * 3 + col * 3];
				sum_g = img_data[row * img.cols * 3 + col * 3 + 1] + lap_data[row * lapla_outimage.cols * 3 + col * 3 + 1];
				sum_r = img_data[row * img.cols * 3 + col * 3 + 2] + lap_data[row * lapla_outimage.cols * 3 + col * 3 + 2];
				if (sum_b > 255) sum_b = 255;
				else if (sum_b < 0) sum_b = 0;
				if (sum_g > 255) sum_g = 255;
				else if (sum_g < 0) sum_g = 0;
				if (sum_r > 255) sum_r = 255;
				else if (sum_r < 0) sum_r = 0;
				output_data[row * output_img.cols * 3 + col * 3] = sum_b;
				output_data[row * output_img.cols * 3 + col * 3 + 1] = sum_g;
				output_data[row * output_img.cols * 3 + col * 3 + 2] = sum_r;
			}
		}
	}
	else if (img.channels() == 1) {
		Mat lapla_outimage(img.rows, img.cols, CV_8UC1, Scalar(0));
		filter(img, lapla_outimage, lapla_mask, 3, 3);
		lap_data = lapla_outimage.data;
		for (int row = 0; row < img.rows; row++) {
			for (int col = 0; col < img.cols; col++) {
				float sum = 0;
				sum = img_data[row * img.cols + col] + lap_data[row * lapla_outimage.cols + col];
				if (sum > 255) sum = 255;
				else if (sum < 0) sum = 0;
				output_data[row * output_img.cols + col] = sum;
			}
		}
	}
}


int main() {
	Mat color_img = imread("겨울이.jpg"); //컬러이미지
	int h = color_img.rows;
	int w = color_img.cols;
	Mat black_img; //흑백이미지
	cvtColor(color_img, black_img, cv::COLOR_RGB2GRAY);


	//float aver_data[] = {		//5X5 moving average
	//1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
	//1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
	//1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
	//1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,
	//1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f,1 / 25.f
	//};

	//float aver_data[] = {		//9X9 moving average
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f, 1 / 81.f,
	//};

	// 3X3 moving average
	float aver_data[] = {
		1 / 9.f,1 / 9.f,1 / 9.f,
		1 / 9.f,1 / 9.f,1 / 9.f,
		1 / 9.f,1 / 9.f,1 / 9.f
	};

	Mat aver_mask(3, 3, CV_32F, aver_data);

	Mat color_aver_outimage(h, w, CV_8UC3, Scalar(0));
	filter(color_img, color_aver_outimage, aver_mask, 3, 3);
	Mat black_aver_outimage(h, w, CV_8UC1, Scalar(0));
	filter(black_img, black_aver_outimage, aver_mask, 3, 3);

	/*imshow("color", color_img), imshow("black", black_img);
	imshow("color_average", color_aver_outimage), imshow("black_average", black_aver_outimage);
	waitKey();*/

	//라플라시안 수행
	float laplacian_data[] = {
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1
	};
	Mat lapla_mask(3, 3, CV_32F, laplacian_data);
	Mat color_lapla_outimage(h, w, CV_8UC3, Scalar(0));
	filter(color_img, color_lapla_outimage, lapla_mask, 3, 3);
	Mat black_lapla_outimage(h, w, CV_8UC1, Scalar(0));
	filter(black_img, black_lapla_outimage, lapla_mask, 3, 3);

	/*imshow("color", color_img), imshow("black", black_img);
	imshow("color_lapla", color_lapla_outimage), imshow("black_lapla", black_lapla_outimage);
	waitKey();*/


	//이미지 샤프닝
	Mat color_sharp_outimage(h, w, CV_8UC3, Scalar(0));
	sharpening(color_img, color_sharp_outimage);
	Mat black_sharp_outimage(h, w, CV_8UC1, Scalar(0));
	sharpening(black_img, black_sharp_outimage);

	imshow("color", color_img), imshow("black", black_img);
	imshow("color_average", color_aver_outimage), imshow("black_average", black_aver_outimage);
	imshow("color_lapla", color_lapla_outimage), imshow("black_lapla", black_lapla_outimage);
	imshow("color_sharp", color_sharp_outimage), imshow("black_sharp", black_sharp_outimage);;
	waitKey();

}
