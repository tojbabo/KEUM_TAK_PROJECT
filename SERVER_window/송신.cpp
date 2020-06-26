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
		if (buf[0] == '#') {														// �� ���̵� Ȯ��
			sscanf(buf, "#ID:%d,PORT:%d", &id, &my_port);
			send(my_port);
		}
		else if (buf[0] == '$') {													// ���ο� Ŭ���̾�Ʈ ����
			printf("���� Ȯ��\n");/*
			sscanf(buf, "$NEW:%d,PORT:%d", &target_id, &target_port);					// �ٸ� ������ ���̵�� ��Ʈ��ȣ �޾ƿ�
			sprintf(buf, "$%d", target_id);											// Ư�� ������ ���ؼ� ��û
			send(sock, buf, strlen(buf), 0);										//
			puts("send data ");
		}
		else if (buf[0] == '@') {
			sscanf(buf, "@PORT:%d", &target_port);									// �̹����� ���� ��Ʈ��ȣ �޾ƿ�
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

	// mat �����͸� jpeg�� ��ȯ�� ������ ��
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
											 // �̹��� ������ ��ȯ
		resize(frame, temp, Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, INTER_LINEAR);

		// jpg�� ��ȯ�� �����Ͱ� ����� ����
		vector < int > compression_params;
		// ���� ����
		compression_params.push_back(IMWRITE_JPEG_QUALITY);
		compression_params.push_back(jpegqual);
		// ������ ��ȯ
		imencode(".jpg", temp, encoded, compression_params);
		imshow("send", temp);
		// ���۵� ��Ŷ�� ���� ����
		int total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

		int ibuf[1];
		ibuf[0] = total_pack;

		// ���ۿ� �ռ� ��Ŷ �� ����
		send(sock, (char*)ibuf, sizeof(int), 0);

		// ������ ��Ŷ �� ��ŭ ������ ����
		for (int i = 0; i < total_pack; i++)
			send(sock, (char*)&encoded[i*PACK_SIZE], PACK_SIZE, 0);
		waitKey(FRAME_INTERVAL);
		
		//puts("sending");
	}
	return 0;
}
