#include"pch.h"
using namespace cv;
using namespace std;

#define DLL_VER "1.6.0"
#define RECENT " - 함수 정리"

void OJJJ_Memset(SOCKADDR_IN *adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}
extern "C" {
	__declspec(dllexport) void Hi() {
		cout << "dll version is : " << DLL_VER << RECENT << endl<<endl;

		WSADATA wsaData;
		if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
			exit(0);
	}

	// 소켓 생성 - 연결 후 반환 // opt_0  : tcp, opt_1 : udp
	__declspec(dllexport) int dll_Get_Socket(String serv_ip, int serv_port, int opt) {
		// opt-0 : tcp, opt-1 : udp
		cout << "이거 출력 안되면 에러 발생할 것임 : - " << serv_ip << endl;
		SOCKADDR_IN adr;
		SOCKET sock = -1;

		OJJJ_Memset(&adr, serv_ip.c_str(), serv_port);

		if (opt == 1) sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		else if (opt == 0)sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		int a = connect(sock, (struct sockaddr*) & adr, sizeof(adr));
		cout << "연결 후 - " << a << endl;
		if (a == -1) {
			cout << "소켓 생성에 에러가 발생했습니다." << endl;
			//closesocket(sock);
			return -1;
		}
		return sock;
	}

	// 소켓 릴리즈
	__declspec(dllexport) void dll_Free_Socket(SOCKET sock) {
		closesocket(sock);
		cout << "소켓 삭제 완료 [" << sock << "]" << endl;
	}

	// C++ 소켓을 통해 메시지를 보내는 함수
	__declspec(dllexport) int DLL_SENDING(int sock, char* msg, int str_len) {
		return send(sock, msg, str_len, 0);
	}

	// 이미지를 C++ 소켓을 통해 전송
	__declspec(dllexport) bool DLL_IMG_SEND(BYTE* b1, int sock)
	{
		Mat frame = Mat(HEIGHT, WIDTH, CV_8UC3, b1);
		Mat temp;
		vector<uchar> encoded;

		if (frame.size().width == 0) return true;
		resize(frame, temp, Size(WIDTH, HEIGHT), 0, 0, INTER_LINEAR);

		vector < int > compression_params;
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(ENCODE_QUALITY);
		imencode(".jpg", temp, encoded, compression_params);

		int total_pack = 1 + (encoded.size() - 1) / PACK_SZ;

		int ibuf[1];
		ibuf[0] = total_pack;

		int len;
		// 전송에 앞서 패킷 수 통지
		len = send(sock, (char*)ibuf, sizeof(int), 0);
		if (len == 0 || len == -1)
			return false;


		// 통지한 패킷 수 만큼 데이터 전송
		for (int i = 0; i < total_pack; i++) {
			len = send(sock, (char*)&encoded[i * PACK_SZ], PACK_SZ, 0);
			if (len == 0 || len == -1)
				return false;
		}

		//waitKey(FRAME_INTERVAL);
		//puts("sending");
		return true;
	}

	// 이미지를 C++ 소켓을 통해 수신
	__declspec(dllexport) Mat* DLL_IMG_RECV(int sock)
	{
		char* msg = new char[BUF_LEN];
		int len;
		do {
			len = recv(sock, msg, BUF_LEN, 0);
			if (len > 20) {
				continue;
			}
			else if (len == 0 || len == -1) {
				return NULL;
			}
		} while (len > sizeof(int));

		int total_pack = ((int*)msg)[0];
		int img_sz = PACK_SZ * total_pack;

		char* longbuf = new char[img_sz];

		int max_sz = 0;
		while (1) {
			len = recv(sock, msg, BUF_LEN, 0);
			//cout << "recv msg : <" << len << "/" << img_sz << "> ";
			memcpy(&longbuf[max_sz], msg, len);
			if (len == 0 || len == -1) {
				free(longbuf);
				return NULL;
			}

			if (max_sz + len < img_sz) {
				//cout << "데이터 계속 받는 중" << endl;
				memcpy(&longbuf[max_sz], msg, len);
				max_sz += len;
			}
			else if (max_sz + len == img_sz) {
				//cout << "모든 데이터 받음" << endl;
				memcpy(&longbuf[max_sz], msg, len);
				break;
			}
			else {
				//cout << "데이터 넘어갔습니다. 다시 시작하겠습니다." << endl;
				free(longbuf);
				return NULL;
			}
		}



		Mat rawData = Mat(1, PACK_SZ * total_pack, CV_8UC1, longbuf);

		Mat mat = imdecode(rawData, IMREAD_COLOR);

		free(longbuf);
		if (mat.size().width != 320 && mat.size().height != 240) {
			cout << "데이터 사이즈 틀림" << endl;
			return NULL;
		}

		return new Mat(mat);
	}
}