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



	cout << "수신ㅇ\n";
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(0);

	SOCKADDR_IN adr;
	OJJJ_Memset(&adr, "0.0.0.0", PORT);


	char msg[BUF_LEN];

	SOCKADDR_IN serv;
	SOCKET sock;

	memset(&serv, 0, sizeof(serv));
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (::bind(sock, (SOCKADDR*)&adr, sizeof(adr)) == SOCKET_ERROR) {
		cout << "바인드 에러";
	}

	if (::listen(sock, 1)) {
		cout << "리슨 에러";
	}
	int adr_sz = sizeof(serv);
	SOCKET sock_ = accept(sock, (SOCKADDR*)&serv, &adr_sz);
	if (sock_ == INVALID_SOCKET)
		cout << "엑셉트 에러";



	int len;
	int serv_len = sizeof(serv);
	int max_sz;

	cout << "받기 시작하는 중" << endl;
	while (1) {
		do {
			len = recv(sock_, msg, BUF_LEN, 0);
		} while (len > sizeof(int));

		int total_pack = ((int*)msg)[0];

		cout << "수신할 패킷 개수 : " << total_pack << endl;
		// 패킷의 크기와 수만큼 변수 생성
		char* longbuf = new char[PACK_SIZE * total_pack];

		max_sz = 0;
		while (1) {
			len = recv(sock_, msg, BUF_LEN, 0);
			cout << "recv msg : <" << len << "/" << total_pack * PACK_SIZE << ">" << endl;
			memcpy(&longbuf[max_sz], msg, len);

			max_sz += len;
			if (max_sz == total_pack * PACK_SIZE)
				break;
			cout << "추가 전송중 \n";
		}

		Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
		Mat frame = imdecode(rawData, IMREAD_COLOR);

		if (frame.size().width == 0) {
			cerr << "decode failure!" << endl;
			continue;
		}

		// 출력
		imshow("recv", frame);
		waitKey(1000 / fps);

		//printf("[RECEIVER] saving in :				%d\n", index);

		free(longbuf);

	}
	closesocket(sock);
	closesocket(sock_);
	puts("i'm out");
	return 0;
}
void OJJJ_Memset(SOCKADDR_IN* adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}