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
												 // �̹��� ������ ��ȯ
			//resize(frame, temp, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);

			// jpg�� ��ȯ�� �����Ͱ� ����� ����
			vector < int > compression_params;
			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(ENCODE_QUALITY);
			// ������ ��ȯ
			imencode(".jpg", frame, encoded, compression_params);
			imshow("send", frame);
			// ���۵� ��Ŷ�� ���� ����
			int total_pack = 1 + (encoded.size() - 1) / PACK_SZ;

			int ibuf[1];
			ibuf[0] = total_pack;

			// ���ۿ� �ռ� ��Ŷ �� ����
			send(socket_no, (char*)ibuf, sizeof(int), 0);

			// ������ ��Ŷ �� ��ŭ ������ ����
			for (int i = 0; i < total_pack; i++)
				send(socket_no, (char*)&encoded[i*PACK_SZ], PACK_SZ, 0);
			waitKey(FRAME_INTERVAL);

			puts("sending");
		}
	}
}
