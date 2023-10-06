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
	Mat input_img = imread("겨울이.jpg");
	int k = 10;
	imshow("input", input_img);
	Mat result = K_Means_Cluster(input_img, k, 5);

	imshow("result", result);
	waitKey();
}

Mat K_Means_Cluster(Mat input_image, int K_Cluster, int D) { //D는 중심점 간의 거리 제한용
	Mat result = Mat::zeros(input_image.size(), CV_8UC3); //결과를 복사하기 위한 result mat 생성

	// # (B, G, R) 3차원 형태로 변환
	uchar* sImage = input_image.data;
	vector <Point3f> point;
	for (int r = 0; r < input_image.rows; r++) {
		for (int c = 0; c < input_image.cols; c++) {
			int offset = r * input_image.cols * 3 + c * 3;
			point.push_back(Point3f(sImage[offset + 0], sImage[offset + 1], sImage[offset + 2])); //BGR값 3차원 공간상에 입력
		}
	}

	// # 초기 군집 중심점 center_point를 설정
	//랜덤하게 설정
	vector<Point3f> center_point;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < K_Cluster; i++) { //K개의 군집 중심점 결정
		int rand1, rand2, rand3; // ==3차원 공간상의 랜덤 x, y, z(0~255 = RGB값범위)
		rand1 = rand() % 255;
		rand2 = rand() % 255;
		rand3 = rand() % 255;
		center_point.push_back(Point3f(rand1, rand2, rand3)); //중심점 3차원 공간상에 입력
	}

	// # 군집에 해당하는 값들을 누적합 하기위한 vector 선언
	vector<vector<int>> calc(center_point.size());
	bool center_is_first = true; //처음으로 결정된 군집 중심점인지 확인을 위한 변수
	while (1) {
		bool is_changed = true; // 군집 중심점이 바뀌었는지 확인을 위한 변수

		// # 모든 값들을 한바퀴 돌면서 가장 가까운 군집 중심에 배정 
		for (int i = 0; i < point.size(); i++) {
			int min_dist = MAX_DIST; //가장 가까운 군집 중심 저장을 위한 변수
			int min_center = -1; //무슨 군집에 속하는지 기억을 위한 변수
			for (int j = 0; j < center_point.size(); j++) {
				int distance; //군집 중심들과의 거리저장을 위한 변수
				distance = sqrt(pow(point[i].x - center_point[j].x, 2) + pow(point[i].y - center_point[j].y, 2) + pow(point[i].z - center_point[j].z, 2)); //군집 중심들과의 거리계산
				if (distance < min_dist) {
					min_dist = distance; //가장 가까운 군집 중심 저장
					min_center = j; // 가장 가까운 군집 j번째를 기억
				}
			}
			calc[min_center].push_back(i); // 하나의 point에서 모든 군집 중심중에 가장 가까운 군집 중심에 point[i]가 속하게 된다.
		}

		// # 현재 배정된 값들의 중점을 구하여 임시 중심점으로 재저장
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
			else { //다 합산후 평균
				b /= calc[i].size();
				g /= calc[i].size();
				r /= calc[i].size();
			}

			temp_center_point[i].x = b;
			temp_center_point[i].y = g;
			temp_center_point[i].z = r;
			cout << b << ", " << g << ", " << r << "\n"; //배정이 되어가는지 중간 확인용
		}

		cout << "\n\n\n";

		// # 위 과정이 첫 계산이었다면, center_point들은 랜덤으로 배정된것.
		// # 두번째 계산 부터는, 새롭게 계산된 중심점인 temp_center_point와 거리 계산.
		int Change = 0; //새롭게 바뀐 중심점과의 거리 계산 저장
		for (int i = 0; i < calc.size(); i++) {
			Change += sqrt(pow(temp_center_point[i].x - center_point[i].x, 2) + pow(temp_center_point[i].y - center_point[i].y, 2) + pow(temp_center_point[i].z - center_point[i].z, 2));
		}

		// # 중심점들끼리의 거리가 같을 수도 있음. 어느 정도 가까운 건 상관없지만, 너무 가깝거나 같은 경우에는 다시 루프를 돌게 해야함.
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

		// # 중심점 첫 배정이면 다음으로 진행, 첫 배정이 아니면 이전 루프의 배정과 같은지 비교
		if (center_is_first == true) {
			center_is_first = false;
			continue;
		}
		else if (Change <= D && is_same == false) { // 현재 중심점들과 이전 중심점들과의 거리 차이가 일정 D 이하면 종료한다.
			break;
		}
		else { //바뀐 군집 중심점으로 재배정
			for (int i = 0; i < center_point.size(); i++) {
				center_point[i].x = temp_center_point[i].x;
				center_point[i].y = temp_center_point[i].y;
				center_point[i].z = temp_center_point[i].z;
			}
		}
	}

	// # 최종 군집 중심점들을 이용하여 색상 변환
	uchar* result_Image = result.data;
	for (int i = 0; i < calc.size(); i++) {
		for (int j = 0; j < calc[i].size(); j++) {
			result_Image[calc[i][j] * 3 + 0] = center_point[i].x; //B
			result_Image[calc[i][j] * 3 + 1] = center_point[i].y; //G
			result_Image[calc[i][j] * 3 + 2] = center_point[i].z; //R
		}
	}

	return result; //결과 이미지 반환

}