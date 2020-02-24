#include"pch.h"
using namespace cv;
using namespace std;

#define DLL_VER "1.0"

void OJJJ_Memset(SOCKADDR_IN *adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}
extern "C" {
	__declspec(dllexport) void Hi() {
		cout << "dll version is : " << DLL_VER << endl;
	}
	__declspec(dllexport) void dll_IMG_SEND_THREAD(String serv_ip,int serv_port)
	{
		SOCKADDR_IN adr;
		SOCKET sock;
		Mat frame;
		Mat temp;
		VideoCapture cap(0);
		vector<uchar> encoded;
		
		OJJJ_Memset(&adr, serv_ip.c_str(), serv_port);
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		connect(sock, (struct sockaddr*)&adr, sizeof(adr));

		namedWindow("send", WINDOW_AUTOSIZE);
		if (!cap.isOpened()) {
			cerr << "OpenCV Failed to open camera";
			exit(1);
		}

		cout << "[SEND THREAD] - sending start" << endl;
		while (1) {
			cap >> frame;
			if (frame.size().width == 0)continue;//simple integrity check; skip erroneous data...
												 // �̹��� ������ ��ȯ
			resize(frame, temp, Size(WIDTH, HEIGHT), 0, 0, INTER_LINEAR);

			// jpg�� ��ȯ�� �����Ͱ� ����� ����
			vector < int > compression_params;
			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(ENCODE_QUALITY);
			// ������ ��ȯ
			imencode(".jpg", temp, encoded, compression_params);
			imshow("send", temp);
			// ���۵� ��Ŷ�� ���� ����
			int total_pack = 1 + (encoded.size() - 1) / PACK_SZ;

			int ibuf[1];
			ibuf[0] = total_pack;

			// ���ۿ� �ռ� ��Ŷ �� ����
			send(sock, (char*)ibuf, sizeof(int), 0);

			// ������ ��Ŷ �� ��ŭ ������ ����
			for (int i = 0; i < total_pack; i++)
				send(sock, (char*)&encoded[i*PACK_SZ], PACK_SZ, 0);
			waitKey(FRAME_INTERVAL);

			//puts("sending");
		}
	}
	__declspec(dllexport) void dll_IMG_RECV_THREAD(String serv_ip, int serv_port)
	{
		char msg[BUF_LEN];
		SOCKADDR_IN serv;
		SOCKET sock;
		int len;

		OJJJ_Memset(&serv, serv_ip.c_str(), serv_port);
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		connect(sock, (struct sockaddr*)&serv, sizeof(serv));
		sprintf(msg, "SEND thread trigger ");
		send(sock, msg, strlen(msg), 0);
		printf("I'm send data : %s to %d\n", msg, serv_port);
		puts("receving transmission");
		while (1) {
			do {
				len = recv(sock, msg, BUF_LEN, 0);
			} while (len > sizeof(int));

			int total_pack = ((int *)msg)[0];

			cout << "expecting length of packs:" << total_pack << endl;
			// ��Ŷ�� ũ��� ����ŭ ���� ����
			char * longbuf = new char[PACK_SZ * total_pack];

			// ��Ŷ�� �� ��ŭ ������ ���ú�
			for (int i = 0; i < total_pack; i++) {
				len = recv(sock, msg, BUF_LEN, 0);
				// ��Ŷ ������� ���� �����͸� ���Ž� ����
				if (len != PACK_SZ) {
					cerr << "Received unexpected size pack:" << len << endl;
					continue;
				}
				// �����ؼ� ���ú��� �����͸� �ϳ��� ������ ����
				memcpy(&longbuf[i * PACK_SZ], msg, PACK_SZ);
			}

			Mat rawData = Mat(1, PACK_SZ * total_pack, CV_8UC1, longbuf);
			Mat frame = imdecode(rawData, IMREAD_COLOR);

			if (frame.size().width == 0) {
				cerr << "decode failure!" << endl;
				continue;
			}

			// ���
			imshow("recv", frame);
			waitKey(1);
			free(longbuf);

		}
		puts("i'm out");
	}
}
