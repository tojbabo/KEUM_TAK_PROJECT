#include"pch.h"
using namespace cv;
using namespace std;

void OJJJ_Memset(SOCKADDR_IN *adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}

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
	__declspec(dllexport) void dll_IMG_SEND_THREAD(String serv_ip,int serv_port)
	{
		SOCKADDR_IN adr;
		SOCKET sock;
		Mat frame;
		VideoCapture cap(0);
		vector<uchar> encoded;
		
		OJJJ_Memset(&adr, serv_ip.c_str(), serv_port);
		cout << "[SEND THREAD] - Memset ok"<<endl;
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		connect(sock, (struct sockaddr*)&adr, sizeof(adr));
		cout << "[SEND THREAD] - connect ok" << endl;

		namedWindow("send", WINDOW_AUTOSIZE);
		if (!cap.isOpened()) {
			cerr << "OpenCV Failed to open camera";
			exit(1);
		}

		cout << "[SEND THREAD] - sending start" << endl;
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
			send(sock, (char*)ibuf, sizeof(int), 0);

			// 통지한 패킷 수 만큼 데이터 전송
			for (int i = 0; i < total_pack; i++)
				send(sock, (char*)&encoded[i*PACK_SZ], PACK_SZ, 0);
			waitKey(FRAME_INTERVAL);

			puts("sending");
		}
	}
	__declspec(dllexport) void dll_IMG_RECV_THREAD(String serv_ip, int serv_port)
	{
	}

	


}
