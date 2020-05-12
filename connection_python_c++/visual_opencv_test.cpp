#include <iostream>
#include <cstring>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;

int main() {
	Mat img = imread("lena.png", IMREAD_COLOR);
	if (img.empty()) {
		cout << "no image" << endl;
		return -1;
	}

	// 가로 세로 - lena.png = 330 * 330
	int cols, rows;
	cols = img.cols;
	rows = img.rows;


	namedWindow("wow", WINDOW_AUTOSIZE);
	imshow("wow", img);

	// 변환
	uchar* data = img.data;
	

	waitKey(0);
}