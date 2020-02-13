#include"pch.h"
using namespace cv;
using namespace std;

extern "C" {
	__declspec(dllexport) void test()
	{
		for (int i = 0; i < 10; i++) {
			Sleep(1000);
			printf("good! %d\n", i);
		}
	}
	__declspec(dllexport) void testwo()
	{
		Mat frame;
		VideoCapture cap(0);

		while (1) {
			cap.read(frame);
			imshow("dfdf", frame);
			if(waitKey(5) >= 0)
				break;
		}

	}
	__declspec(dllexport) void dll_IMG_SEND_THREAD(int socket_no)
	{
		Mat frame;
		VideoCapture cap(0);

		vector<uchar> encoded;

		namedWindow("send", WINDOW_AUTOSIZE);
		if (!cap.isOpened()) {
			cerr << "OpenCV Failed to open camera";
			exit(1);
		}

		while (1) {
			cap >> frame;
			if (frame.size().width == 0)continue;//simple integrity check; skip erroneous data...
												 // 이미지 사이즈 변환
			//resize(frame, temp, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);

			// jpg로 변환된 데이터가 저장될 변수
			vector < int > compression_params;
			// 변수 설정
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(ENCODE_QUALITY);
			// 데이터 변환
			imencode(".jpg", frame, encoded, compression_params);
			imshow("send", frame);
			// 전송될 패킷의 숫자 설정
			int total_pack = 1 + (encoded.size() - 1) / PACK_SZ;

			int ibuf[1];
			ibuf[0] = total_pack;

			// 전송에 앞서 패킷 수 통지
			send(socket_no, (char*)ibuf, sizeof(int), 0);

			// 통지한 패킷 수 만큼 데이터 전송
			for (int i = 0; i < total_pack; i++)
				send(socket_no, (char*)&encoded[i*PACK_SZ], PACK_SZ, 0);
			waitKey(FRAME_INTERVAL);

			puts("sending");
		}
	}
}
