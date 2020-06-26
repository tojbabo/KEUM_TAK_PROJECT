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
		if (buf[0] == '#') {														// �� ���̵� Ȯ��
			sscanf(buf, "#ID:%d,PORT:%d", &id, &my_port);
		}
		else if (buf[0] == '$') {													// ���ο� Ŭ���̾�Ʈ ����
			printf("���� Ȯ��\n");
			sscanf(buf, "$NEW:%d,PORT:%d", &target_id, &target_port);					// �ٸ� ������ ���̵�� ��Ʈ��ȣ �޾ƿ�
			sprintf(buf, "$%d", target_id);											// Ư�� ������ ���ؼ� ��û
			send(sock, buf, strlen(buf), 0);										//
			puts("send data ");
		}
		else if (buf[0] == '@') {
			sscanf(buf, "@PORT:%d", &target_port);									// �̹����� ���� ��Ʈ��ȣ �޾ƿ�
			OJJJ_recv(target_port);
		}
		}*/
		ptr = strtok(buf, "\n");

		while (ptr != NULL) {
			ptr[len] = 0;
			printf("recv data : %s\n", ptr);
			if (ptr[0] == '#') {														// �� ���̵� Ȯ��
				sscanf(ptr, "#ID:%d,PORT:%d", &id, &my_port);
			}
			else if (ptr[0] == '$') {													// ���ο� Ŭ���̾�Ʈ ����
				printf("���� Ȯ��\n");
				sscanf(ptr, "$NEW:%d,PORT:%d", &target_id, &target_port);				// �ٸ� ������ ���̵�� ��Ʈ��ȣ �޾ƿ�
				sprintf(msg, "$%d", target_id);											// Ư�� ������ ���ؼ� ��û
				send(sock, msg, strlen(buf), 0);										//
				puts("send data ");
			}
			else if (ptr[0] == '@') {
				sscanf(ptr, "@PORT:%d", &target_port);									// �̹����� ���� ��Ʈ��ȣ �޾ƿ�
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
		// ��Ŷ�� ũ��� ����ŭ ���� ����
		char * longbuf = new char[PACK_SIZE * total_pack];

		// ��Ŷ�� �� ��ŭ ������ ���ú�
		for (int i = 0; i < total_pack; i++) {
			len = recv(sock, msg, BUF_LEN, 0);
			// ��Ŷ ������� ���� �����͸� ���Ž� ����
			if ( len != PACK_SIZE) {
				cerr << "Received unexpected size pack:" << len << endl;
				continue;
			}
			// �����ؼ� ���ú��� �����͸� �ϳ��� ������ ����
			memcpy(&longbuf[i * PACK_SIZE], msg, PACK_SIZE);
		}

		Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
		Mat frame = imdecode(rawData, IMREAD_COLOR);

		if (frame.size().width == 0) {
			cerr << "decode failure!" << endl;
			continue;
		}

		// ���
		imshow("recv", frame);
		waitKey(1);

		//printf("[RECEIVER] saving in :				%d\n", index);

		free(longbuf);
		
	}
	puts("i'm out");
	return 0;
}
