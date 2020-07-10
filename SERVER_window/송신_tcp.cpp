#include "OJJJ.h"
using namespace std;
using namespace cv;

void OJJJ_Memset(SOCKADDR_IN*, const char*, int);
int main() {

	FILE* stream;
	stream = fopen("config.txt", "r");
	char reader[100];
	fread(reader, 100, 1, stream);
	char* ip = get_value(reader, strlen(reader), "ip:", strlen("ip:"));
	int fps;
	char* fps_ = get_value(reader, strlen(reader), "fps:", strlen("fps:"));
	fps = atoi(fps_);
	cout << "ip is : " << ip << endl;
	cout << "fps is : " << fps << endl;
	fclose(stream);

	cout << "송신자\n";
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(-1);

	SOCKADDR_IN serv;
	SOCKET sock;

	OJJJ_Memset(&serv, ip, PORT);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(sock, (struct sockaddr*) & serv, sizeof(serv));

	int jpegqual = ENCODE_QUALITY; // Compression Parameter

	Mat frame, temp;

	vector < uchar > encoded;
	VideoCapture cap(0); // Grab the camera


	namedWindow("send", WINDOW_AUTOSIZE);
	if (!cap.isOpened()) {
		cerr << "OpenCV Failed to open camera";
		exit(1);
	}

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
			send(sock, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0);
		waitKey(1000 / fps);

		//puts("sending");
	}
	return 0;

}
void OJJJ_Memset(SOCKADDR_IN* adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}