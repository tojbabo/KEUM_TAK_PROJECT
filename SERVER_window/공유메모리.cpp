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


#define memory "I01_0"
#define file "KEY1"

using namespace std;
using namespace cv;

void SH_print();
void SH_CapAndWrite();
void SH_ReadAndShow();

LPCSTR CreateMemory(HANDLE* h, TCHAR szName[]) {
	//TCHAR szName[] = TEXT(memory);
	LPCSTR SM;

	*h = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size1,	                 // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (*h == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return NULL;
	}

	SM = (LPTSTR)MapViewOfFile(*h,   // handle to map object
		FILE_MAP_ALL_ACCESS,				 // read/write permission
		0,
		0,
		size1);

	if (SM == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(*h);

		return NULL;
	}

	return SM;
}

char* TenToTwo(int ten,int bit) {
	char* result;
	result = (char*)malloc(sizeof(char) * bit+1);
	result[bit] = 0;
	int a=ten;
	int b;

	for (int i = bit - 1; i >= 0; i--) {
		b = a % 2;
		a = a / 2;
		result[i] = b + '0';
	}
	return result;
}
int TwoToTen(char* two,int bit) {
	int result=0;
	int a = 1;
	for (int i = bit - 1; i >= 0; i--) {
		result += (two[i] - '0') * a;
		a *= 2;
	}
	return result;
}
char* StrAdd(char* a, char* b) {
	int ai;
	for (ai = 0;; ai++) {
		if (a[ai] == 0) {
			break;
		}
	}

	int bi;
	for (bi = 0;; bi++) {
		if (b[bi] == 0) {
			break;
		}
	}

	char* r = (char*)malloc(sizeof(char) * (ai + bi+1));

	for (int i = 0; i < ai; i++) {
		r[i] = a[i];
	}
	for (int i = 0; i < bi; i++) {
		r[ai+i] = b[i];
	}
	r[ai + bi] == 0;
	return r;
}



int main() {

	int num;
	cout << "1, 2 : [" << memory << "] , 3 : [" << file << "]" << endl;
	cout << "트리거 0일땐 wait, 아닐땐 0으로 바꿈, 1로 만들어 놓고 시작시킴" << endl;
	
	printf("<선택>\n1 : 촬영-메모리쓰기  \n2 : 메모리읽기-출력 \n3 : print\n>> ");
	scanf("%d", &num);
	if (num == 1) {
		SH_CapAndWrite();
	}
	else if (num == 2)
		SH_ReadAndShow();
	else
		SH_print();
}
void SH_CapAndWrite() {

	TCHAR szName[] = TEXT(memory);

	HANDLE hMapFile;
	LPCTSTR SM = CreateMemory(&hMapFile,szName);

	if (hMapFile == NULL) return;
	if (SM == NULL) return;

	int size;
	Mat frame;

	Mat temp;

	vector<uchar>encoded;

	vector < int > compression_params;

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		printf("error : cap open\n");
		exit(1);
	}

	while (1) {
		cap >> frame;
		resize(frame, temp, Size(320, 240), 0, 0, INTER_LINEAR);

		size = 1 + (encoded.size() - 1);
		if (frame.size().width == 0) {
			continue;
		}
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(ENCODE_QUALITY);
		imencode(".jpg", temp, encoded, compression_params);

		//CopyMemory((PVOID)SM, &temp, sizeof(Mat));
		CopyMemory((PVOID)SM, &encoded[0], size);

		/*
		char* binary_size = TenToTwo(encoded.size(), 16);
		int size_b = TwoToTen(binary_size + 8, 8);
		binary_size[8] = 0;
		int size_a = TwoToTen(binary_size, 8);


		vector<uchar>  data;
		data.push_back(size_a);
		data.push_back(size_b);

		data.insert(data.end(), encoded.begin(), encoded.end());

		CopyMemory((PVOID)SM, &data[0], data.size());*/


		cout << "메모리에 성공적으로 썼음!" << "["/* << data.size()*/ << "/" << encoded.size() << "]" << endl;

		waitKey(FRAME_INTERVAL);
	}
	UnmapViewOfFile(SM);
	CloseHandle(hMapFile);
}
void SH_ReadAndShow() {
	TCHAR szName[] = TEXT(memory);

	HANDLE hMapFile;
	LPCTSTR SM = CreateMemory(&hMapFile, szName);
	if (hMapFile == NULL) return;
	if (SM == NULL) return;

	int size;
	TCHAR buf[100];
	Mat frame;
	BYTE* b;

	vector<uchar> encoded;
	Mat temp;
	Mat mat;
	while (1) {
		/*
		char* z = TenToTwo((int)SM[0], 8);
		char* x = TenToTwo((int)SM[1], 8);
		char* tt = StrAdd(z, x);
		int ReadSize = TwoToTen(tt, 16);
		*/

				
		temp = Mat(1, size1, CV_8UC1, (char*)SM);
		mat = imdecode(temp, IMREAD_COLOR);
		
		if (mat.size().width == 0) {
			cout << mat.size().width << " - is  width" << endl;
			cout << mat.size().height << " - is  height" << endl;
			continue;
		}
		cout << "메모리로부터 성공적으로 읽어옴"<<"["<<"]" << endl;

		imshow("img", mat);

		waitKey(FRAME_INTERVAL);


	}

	UnmapViewOfFile(SM);
	CloseHandle(hMapFile);
}
void SH_print() {

	TCHAR szName[] = TEXT(file);

	HANDLE hMapFile;
	LPCTSTR SM = CreateMemory(&hMapFile,szName);
	if (hMapFile == NULL) return;
	if (SM == NULL) return;


	
	CopyMemory((PVOID)SM, "2,I01_0", (_tcslen("2,I01_0") * sizeof(TCHAR)));
	cout << "메모리를 2로 바꿈 \n";
	// 1 - 내가 메모리에 이미지를 썼을때
	// 2 - 새로운 사용자가 들어왔음을 알렸을때
	// 3 - 대기
	// 0 - 감정 결과가 나왔을때
	while (1) {
		if (SM[0] == '0') {
			cout << "읽은 데이터 : "<<SM << "1로 바꿈"<<endl;
			CopyMemory((PVOID)SM, "1", (_tcslen("1") * sizeof(TCHAR)));
		}
		else if (SM[0] == '2' || SM[0] == '1') {
			cout << "AI가 아직 파일을 확인하지 않음\n";
		}
		else {
			printf("메모리 : %s - 대기 or 미지정\n", SM);
		}
		Sleep(1000);
	}

	UnmapViewOfFile(SM);
	CloseHandle(hMapFile);
}