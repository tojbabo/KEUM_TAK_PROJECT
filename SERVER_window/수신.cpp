#include "OJJJ.h"
using namespace std;
using namespace cv;
 

void OJJJ_Memset(SOCKADDR_IN*, const char*, int);
int main() {
	
	FILE* stream;
	stream = fopen("config.txt", "r");
	char reader[100];
	fread(reader, 100, 1, stream);
	int fps;
	char* fps_ = get_value(reader, strlen(reader), "fps:", strlen("fps:"));
	fps = atoi(fps_);
	cout << "fps is : " << fps << endl;
	fclose(stream);



	cout<< "수신\n";
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(0);

	SOCKADDR_IN adr;
	OJJJ_Memset(&adr, "0.0.0.0", PORT);

		
	char msg[BUF_LEN];

	SOCKADDR_IN serv;
	SOCKET sock;

	memset(&serv, 0, sizeof(serv));
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (::bind(sock, (SOCKADDR*)&adr, sizeof(adr)) == SOCKET_ERROR) {
		cout << "바인드 에러";
	}

	int len;
	int serv_len = sizeof(serv);

	cout << "받기 시작하는 중" << endl;
	while (1) {
		do {
			len = recvfrom(sock, msg, BUF_LEN, 0, (SOCKADDR*)&serv, &serv_len);
		} while (len > sizeof(int));

		int total_pack = ((int*)msg)[0];

		cout << "expecting length of packs:" << total_pack << endl;
		// 패킷의 크기와 수만큼 변수 생성
		char* longbuf = new char[PACK_SIZE * total_pack];

		// 패킷의 수 만큼 데이터 리시브
		for (int i = 0; i < total_pack; i++) {
			len = recvfrom(sock, msg, BUF_LEN, 0, (SOCKADDR*)&serv, &serv_len);
			// 패킷 사이즈보다 작은 데이터를 수신시 무시
			if (len != PACK_SIZE) {
				cerr << "Received unexpected size pack:" << len << endl;
				continue;
			}
			// 연속해서 리시브한 데이터를 하나의 변수로 통합
			memcpy(&longbuf[i * PACK_SIZE], msg, PACK_SIZE);
		}

		Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
		Mat frame = imdecode(rawData, IMREAD_COLOR);

		if (frame.size().width == 0) {
			cerr << "decode failure!" << endl;
			continue;
		}

		// 출력
		imshow("recv", frame);
		waitKey(1000/fps);

		//printf("[RECEIVER] saving in :				%d\n", index);

		free(longbuf);

	}
	puts("i'm out");
	return 0;
}
void OJJJ_Memset(SOCKADDR_IN *adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}