#include "OJJJ.h"
using namespace std;
using namespace cv;

void OJJJ_Memset(SOCKADDR_IN*, const char*, int);
int send(int);
int main() {
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(-1);

	SOCKADDR_IN adr;
	OJJJ_Memset(&adr, IP, PORT);
 
	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(sock, (SOCKADDR*)&adr, sizeof(adr)) == SOCKET_ERROR)
		exit(-2);
	char buf[BUF_SIZE];
	int len;
	puts("connect success");
	int id;
	int my_port;
	while (1) {
		puts("Waiting Recv");
		len = recv(sock, buf, BUF_SIZE, 0);
		buf[len] = 0;
		printf("recv data : %s\n", buf);
		if (buf[0] == '#') {														// 내 아이디 확인
			sscanf(buf, "#ID:%d,PORT:%d", &id, &my_port);
			send(my_port);
		}
		else if (buf[0] == '$') {													// 새로운 클라이언트 접속
			printf("연결 확인\n");/*
			sscanf(buf, "$NEW:%d,PORT:%d", &target_id, &target_port);					// 다른 유저의 아이디와 포트번호 받아옴
			sprintf(buf, "$%d", target_id);											// 특정 유저에 대해서 요청
			send(sock, buf, strlen(buf), 0);										//
			puts("send data ");
		}
		else if (buf[0] == '@') {
			sscanf(buf, "@PORT:%d", &target_port);									// 이미지를 받을 포트번호 받아옴
			OJJJ_recv(target_port);
		}*/
		}
	}
}
void OJJJ_Memset(SOCKADDR_IN *adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}

int send(int port) {
	char msg[10];
	SOCKADDR_IN serv;
	SOCKET sock;

	OJJJ_Memset(&serv, IP, port);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	connect(sock, (struct sockaddr*)&serv, sizeof(serv));

	// mat 데이터를 jpeg로 변환시 설정될 값
	int jpegqual = ENCODE_QUALITY; // Compression Parameter

	Mat frame, temp;

	vector < uchar > encoded;
	VideoCapture cap(0); // Grab the camera


	namedWindow("send", WINDOW_AUTOSIZE);
	if (!cap.isOpened()) {
		cerr << "OpenCV Failed to open camera";
		exit(1);
	}

	clock_t last_cycle = clock();
	while (1) {
		cap >> frame;
		if (frame.size().width == 0)continue;//simple integrity check; skip erroneous data...
											 // 이미지 사이즈 변환
		resize(frame, temp, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);

		// jpg로 변환된 데이터가 저장될 변수
		vector < int > compression_params;
		// 변수 설정
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(jpegqual);
		// 데이터 변환
		imencode(".jpg", temp, encoded, compression_params);
		imshow("send", temp);
		// 전송될 패킷의 숫자 설정
		int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

		int ibuf[1];
		ibuf[0] = total_pack;

		// 전송에 앞서 패킷 수 통지
		send(sock, (char*)ibuf, sizeof(int), 0);

		// 통지한 패킷 수 만큼 데이터 전송
		for (int i = 0; i < total_pack; i++)
			send(sock, (char*)&encoded[i*PACK_SIZE], PACK_SIZE, 0);
		waitKey(FRAME_INTERVAL);
		
		//puts("sending");
	}
	return 0;
}
