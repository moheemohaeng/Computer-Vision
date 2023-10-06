#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <ctime>

using namespace cv;
using namespace std;
#define MAX_DIST 50000

Mat K_Means_Cluster(Mat, int, int);

int main()
{
	Mat input_img = imread("�ܿ���.jpg");
	int k = 10;
	imshow("input", input_img);
	Mat result = K_Means_Cluster(input_img, k, 5);

	imshow("result", result);
	waitKey();
}

Mat K_Means_Cluster(Mat input_image, int K_Cluster, int D) { //D�� �߽��� ���� �Ÿ� ���ѿ�
	Mat result = Mat::zeros(input_image.size(), CV_8UC3); //����� �����ϱ� ���� result mat ����

	// # (B, G, R) 3���� ���·� ��ȯ
	uchar* sImage = input_image.data;
	vector <Point3f> point;
	for (int r = 0; r < input_image.rows; r++) {
		for (int c = 0; c < input_image.cols; c++) {
			int offset = r * input_image.cols * 3 + c * 3;
			point.push_back(Point3f(sImage[offset + 0], sImage[offset + 1], sImage[offset + 2])); //BGR�� 3���� ������ �Է�
		}
	}

	// # �ʱ� ���� �߽��� center_point�� ����
	//�����ϰ� ����
	vector<Point3f> center_point;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < K_Cluster; i++) { //K���� ���� �߽��� ����
		int rand1, rand2, rand3; // ==3���� �������� ���� x, y, z(0~255 = RGB������)
		rand1 = rand() % 255;
		rand2 = rand() % 255;
		rand3 = rand() % 255;
		center_point.push_back(Point3f(rand1, rand2, rand3)); //�߽��� 3���� ������ �Է�
	}

	// # ������ �ش��ϴ� ������ ������ �ϱ����� vector ����
	vector<vector<int>> calc(center_point.size());
	bool center_is_first = true; //ó������ ������ ���� �߽������� Ȯ���� ���� ����
	while (1) {
		bool is_changed = true; // ���� �߽����� �ٲ������ Ȯ���� ���� ����

		// # ��� ������ �ѹ��� ���鼭 ���� ����� ���� �߽ɿ� ���� 
		for (int i = 0; i < point.size(); i++) {
			int min_dist = MAX_DIST; //���� ����� ���� �߽� ������ ���� ����
			int min_center = -1; //���� ������ ���ϴ��� ����� ���� ����
			for (int j = 0; j < center_point.size(); j++) {
				int distance; //���� �߽ɵ���� �Ÿ������� ���� ����
				distance = sqrt(pow(point[i].x - center_point[j].x, 2) + pow(point[i].y - center_point[j].y, 2) + pow(point[i].z - center_point[j].z, 2)); //���� �߽ɵ���� �Ÿ����
				if (distance < min_dist) {
					min_dist = distance; //���� ����� ���� �߽� ����
					min_center = j; // ���� ����� ���� j��°�� ���
				}
			}
			calc[min_center].push_back(i); // �ϳ��� point���� ��� ���� �߽��߿� ���� ����� ���� �߽ɿ� point[i]�� ���ϰ� �ȴ�.
		}

		// # ���� ������ ������ ������ ���Ͽ� �ӽ� �߽������� ������
		vector<Point3f> temp_center_point(center_point.size());
		for (int i = 0; i < calc.size(); i++) {
			int b = 0, g = 0, r = 0;
			for (int j = 0; j < calc[i].size(); j++) {
				b += point[calc[i][j]].x;
				g += point[calc[i][j]].y;
				r += point[calc[i][j]].z;
			}
			if (b == 0)
				b = 0;
			else if (g == 0)
				g = 0;
			else if (r == 0)
				r = 0;
			else { //�� �ջ��� ���
				b /= calc[i].size();
				g /= calc[i].size();
				r /= calc[i].size();
			}

			temp_center_point[i].x = b;
			temp_center_point[i].y = g;
			temp_center_point[i].z = r;
			cout << b << ", " << g << ", " << r << "\n"; //������ �Ǿ���� �߰� Ȯ�ο�
		}

		cout << "\n\n\n";

		// # �� ������ ù ����̾��ٸ�, center_point���� �������� �����Ȱ�.
		// # �ι�° ��� ���ʹ�, ���Ӱ� ���� �߽����� temp_center_point�� �Ÿ� ���.
		int Change = 0; //���Ӱ� �ٲ� �߽������� �Ÿ� ��� ����
		for (int i = 0; i < calc.size(); i++) {
			Change += sqrt(pow(temp_center_point[i].x - center_point[i].x, 2) + pow(temp_center_point[i].y - center_point[i].y, 2) + pow(temp_center_point[i].z - center_point[i].z, 2));
		}

		// # �߽����鳢���� �Ÿ��� ���� ���� ����. ��� ���� ����� �� ���������, �ʹ� �����ų� ���� ��쿡�� �ٽ� ������ ���� �ؾ���.
		bool is_same = false;
		for (int k1 = 0; k1 < calc.size() - 1; k1++) {
			for (int k2 = k1 + 1; k2 < calc.size(); k2++) {
				if (sqrt(pow(temp_center_point[k1].x - temp_center_point[k2].x, 2) + pow(temp_center_point[k1].y - temp_center_point[k2].y, 2) + pow(temp_center_point[k1].z - temp_center_point[k2].z, 2)) <= 5) {
					k1 = calc.size();
					is_same = true;
					break;
				}
			}
		}

		// # �߽��� ù �����̸� �������� ����, ù ������ �ƴϸ� ���� ������ ������ ������ ��
		if (center_is_first == true) {
			center_is_first = false;
			continue;
		}
		else if (Change <= D && is_same == false) { // ���� �߽������ ���� �߽�������� �Ÿ� ���̰� ���� D ���ϸ� �����Ѵ�.
			break;
		}
		else { //�ٲ� ���� �߽������� �����
			for (int i = 0; i < center_point.size(); i++) {
				center_point[i].x = temp_center_point[i].x;
				center_point[i].y = temp_center_point[i].y;
				center_point[i].z = temp_center_point[i].z;
			}
		}
	}

	// # ���� ���� �߽������� �̿��Ͽ� ���� ��ȯ
	uchar* result_Image = result.data;
	for (int i = 0; i < calc.size(); i++) {
		for (int j = 0; j < calc[i].size(); j++) {
			result_Image[calc[i][j] * 3 + 0] = center_point[i].x; //B
			result_Image[calc[i][j] * 3 + 1] = center_point[i].y; //G
			result_Image[calc[i][j] * 3 + 2] = center_point[i].z; //R
		}
	}

	return result; //��� �̹��� ��ȯ

}