#include "OJJJ.h"
using namespace std;
using namespace cv;
 

void OJJJ_Memset(SOCKADDR_IN*, const char*, int);
int OJJJ_recv(int);
int main() {
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(0);

	SOCKADDR_IN adr;
	OJJJ_Memset(&adr, "127.0.0.1", PORT);

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (connect(sock, (SOCKADDR*)&adr, sizeof(adr)) == SOCKET_ERROR)
		exit(1);
	char buf[BUF_SIZE];
	int len;
	puts("connect success");
	send(sock, "^JUN", strlen("^NUN"), 0);
	int id;
	int my_port;
	int target_id,target_port;
	char* ptr;
	char msg[BUF_SIZE];
	while (1) {
		puts("Waiting Recv");
		len = recv(sock, buf, BUF_SIZE, 0);
		/*
		buf[len] = 0;
		printf("recv data : %s\n", buf);
		if (buf[0] == '#') {														// 내 아이디 확인
			sscanf(buf, "#ID:%d,PORT:%d", &id, &my_port);
		}
		else if (buf[0] == '$') {													// 새로운 클라이언트 접속
			printf("연결 확인\n");
			sscanf(buf, "$NEW:%d,PORT:%d", &target_id, &target_port);					// 다른 유저의 아이디와 포트번호 받아옴
			sprintf(buf, "$%d", target_id);											// 특정 유저에 대해서 요청
			send(sock, buf, strlen(buf), 0);										//
			puts("send data ");
		}
		else if (buf[0] == '@') {
			sscanf(buf, "@PORT:%d", &target_port);									// 이미지를 받을 포트번호 받아옴
			OJJJ_recv(target_port);
		}
		}*/
		ptr = strtok(buf, "\n");

		while (ptr != NULL) {
			ptr[len] = 0;
			printf("recv data : %s\n", ptr);
			if (ptr[0] == '#') {														// 내 아이디 확인
				sscanf(ptr, "#ID:%d,PORT:%d", &id, &my_port);
			}
			else if (ptr[0] == '$') {													// 새로운 클라이언트 접속
				printf("연결 확인\n");
				sscanf(ptr, "$NEW:%d,PORT:%d", &target_id, &target_port);				// 다른 유저의 아이디와 포트번호 받아옴
				sprintf(msg, "$%d", target_id);											// 특정 유저에 대해서 요청
				send(sock, msg, strlen(buf), 0);										//
				puts("send data ");
			}
			else if (ptr[0] == '@') {
				sscanf(ptr, "@PORT:%d", &target_port);									// 이미지를 받을 포트번호 받아옴
				OJJJ_recv(target_port);
			}
			ptr = strtok(NULL, "\n");
		}
	}
}
void OJJJ_Memset(SOCKADDR_IN *adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}
int OJJJ_recv(int port) {
	printf("recv socket create ok ! \n");
	char msg[BUF_LEN];
	SOCKADDR_IN serv;
	SOCKET sock;
	int len;

	OJJJ_Memset(&serv, IP, port);
	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	connect(sock, (struct sockaddr*)&serv, sizeof(serv));
	sprintf(msg, "SEND thread trigger ");
	send(sock,msg,strlen(msg),0);
	printf("I'm send data : %s to %d\n", msg, port);
	puts("receving transmission");
	while (1) {
		do {
			len = recv(sock, msg, BUF_LEN, 0);
		} while (len > sizeof(int));

		int total_pack = ((int *)msg)[0];

		cout << "expecting length of packs:" << total_pack << endl;
		// 패킷의 크기와 수만큼 변수 생성
		char * longbuf = new char[PACK_SIZE * total_pack];

		// 패킷의 수 만큼 데이터 리시브
		for (int i = 0; i < total_pack; i++) {
			len = recv(sock, msg, BUF_LEN, 0);
			// 패킷 사이즈보다 작은 데이터를 수신시 무시
			if ( len != PACK_SIZE) {
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
		waitKey(1);

		//printf("[RECEIVER] saving in :				%d\n", index);

		free(longbuf);
		
	}
	puts("i'm out");
	return 0;
}
