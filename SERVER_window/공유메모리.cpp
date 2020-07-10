#include<stdio.h>
#include<Windows.h>
#include<tchar.h>
#include<iostream>
#include<conio.h>
#include<stdlib.h>
#include <opencv2/opencv.hpp>

#define BUF_SIZE 256
#define size1 230400+10
#define size2 921600+10
#define ENCODE_QUALITY 80
#define FRAME_INTERVAL 1000/30
#define PACK_SIZE 4096

using namespace std;
using namespace cv;

void SH_print();
void SH_CapAndWrite();
void SH_ReadAndShow();

int main() {

	int num;
	cout << "1, 2 : I01_0 , 3 : KEY1" << endl;
	cout << "트리거 0일땐 wait, 아닐땐 0으로 바꿈, 1로 만들어 놓고 시작시킴" << endl;
	printf("1 : 촬영-메모리쓰기  / 2 : 메모리읽기-출력 / 3 : print>> ");
	scanf("%d", &num);
	if (num == 1) {
		SH_CapAndWrite();
	}
	else if (num == 2)
		SH_ReadAndShow();
	else
		SH_print();
}
void SH_ReadAndShow() {
	TCHAR szName[] = TEXT("I01_0");

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size1,	                 // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS,				 // read/write permission
		0,
		0,
		size1);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return;
	}

	int size;
	TCHAR buf[100];
	Mat frame;
	BYTE* b;

	vector<uchar> encoded;

	uchar* d;
	Mat rawData;


	while (1) {

		Mat temp;
		temp = Mat(1, size, CV_8UC1, (char*)pBuf);
		temp = imdecode(temp, IMREAD_COLOR);
		Mat mat = Mat(temp);

		if (mat.size().width == 0) {
			cout << mat.size().width << " - is  width" << endl;
			cout << mat.size().height << " - is  height" << endl;
			continue;
		}

		imshow("qweqwe", mat);

		waitKey(FRAME_INTERVAL);


		waitKey(1000 / 30);

	}

	printf("아무키나 누르면 공유메모리 연결 끊고 종료됨 \n");
	system("pause");

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}
void SH_CapAndWrite() {

	TCHAR szName[] = TEXT("I01_0");

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size1,	                 // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS,				 // read/write permission
		0,
		0,
		size1);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return;
	}

	int size;
	Mat frame;
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		printf("error : cap open\n");
		exit(1);
	}

	vector<uchar>encoded;
	vector < int > compression_params;
	int ibuf[1];
	int total_pack;

	while (1) {

		try {
			cap >> frame;
			resize(frame, frame, Size(320, 240), 0, 0, INTER_LINEAR);


			size = 1 + (encoded.size() - 1);
			if (frame.size().width == 0) {
				continue;
			}
			compression_params.push_back(1);
			compression_params.push_back(80);

			imencode(".jpg", frame, encoded, compression_params);

			CopyMemory((PVOID)pBuf, &encoded[0], size);

			waitKey(FRAME_INTERVAL);
		}
		catch (exception e) {
			cout << "catch somthing" << endl;
			continue;
		}
	}
	printf("아무키나 누르면 공유메모리 연결 끊고 종료됨 \n");
	system("pause");

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}
void SH_print() {

	TCHAR szName[] = TEXT("KEY1");

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size1,	                 // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS,				 // read/write permission
		0,
		0,
		size1);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return;
	}
	CopyMemory((PVOID)pBuf, "1", (_tcslen("1") * sizeof(TCHAR)));

	while (1) {
		if (pBuf[0] == '0')
			printf("wait\n");
		else {
			printf("buf is : %s\n", pBuf);
			CopyMemory((PVOID)pBuf, "0,2,-", (_tcslen("0,2,-") * sizeof(TCHAR)));
		}
		Sleep(1000);
	}

	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}