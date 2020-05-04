#include "OJJJ.h"

using namespace cv;
using namespace std;

SOCKET Serv_Initialize(int, int);
void OJJJ_Memset(SOCKADDR_IN*, const char*, int);
void OJJJ_Main_Task();
void OJJJ_Thread_Recv(int, int);
void OJJJ_Thread_Send(int, int);

bool isRECV;
bool isSEND;
bool isPRINT;

bool isSAVE[MAXIMUM_USER] = { false };
Mat MATS[MAXIMUM_USER];
RW_LOCK RW;
COMMAND_CENTER cmd_ctr;

int main() {
	OJJJ_Main_Task();

	return 0;
}

// ���� ���� ���� ������ SOCKET�� ��ȯ�ϴ� �Լ�
SOCKET Serv_Initialize(int port, int opt) {
	// opt -- 1: TCP , 2 : UDP
	SOCKADDR_IN bind_adr;
	OJJJ_Memset(&bind_adr, IP, port);

	SOCKET sock;
	if (opt == 1)
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else if (opt == 2)
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	else {
		cout << "wrong initalize option (only 1 or 2) \n";
		exit(-1);
	}

	if (::bind(sock, (struct sockaddr*) & bind_adr, sizeof(bind_adr)) == SOCKET_ERROR) {
		cout << "binding error \n";
		exit(-1);
	}

	return sock;
}

// �Էµ� ���� ���� �ּ��Լ� �ʱ�ȭ �Լ�
void OJJJ_Memset(SOCKADDR_IN* adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}

// TCP - SELECT ��� �Լ�
void OJJJ_Main_Task() {
	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(0);

	SOCKET sock = Serv_Initialize(PORT, 1);
	if (listen(sock, 5) == SOCKET_ERROR) {
		cout << "listening error \n";
		exit(-1);
	}

	fd_set read, temp;
	TIMEVAL time;
	FD_ZERO(&read); 
	FD_SET(sock, &read);

	int re;
	char buf[BUF_SIZE];
	char msg[BUF_SIZE];
	int connected_client_port;
	thread t[MAXIMUM_USER];
	thread send_t[MAXIMUM_USER];
	int thread_temp = 0;
	int target_id;
	int target_port;

	RW = RW_LOCK();

	printf("���� ���� ��ٸ��� �� \n");
	while (1) {
		temp = read;																			// �ӽ� �迭 ����
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re�� select ����
		if (re == SOCKET_ERROR) break;
		if (re == 0) continue;

		for (int i = 0; i < read.fd_count; i++) {												// select ������ ��ŭ �ݺ�
			if (FD_ISSET(read.fd_array[i], &temp))												// Ư�� ������ �ӽ� �迭�� �����ϴ��� Ȯ��
			{
				if (sock == read.fd_array[i]) {													// ������ ������ ���� ������ ���
					SOCKADDR_IN client_adr;
					int size = sizeof(client_adr);
					SOCKET client_sock = accept(sock, (SOCKADDR*)&client_adr, &size);			// ������ �߰� ����

					FD_SET(client_sock, &read);													// select �迭�� �ش� ���� ����
					printf("[MAIN]New Connect user :			%d\n", client_sock);
					connected_client_port = cmd_ctr.Connect_New_Client(client_sock);
					if (connected_client_port == -1) 
						printf("�� �̻� Ŭ���̾�Ʈ�� ���� �� ����.\n");
					else {
						int temp = cmd_ctr.Get_blank_index();
						t[temp] = thread(OJJJ_Thread_Recv, connected_client_port, cmd_ctr.Get_index(client_sock));
					}
					cmd_ctr.Check_Client(client_sock, connected_client_port);
				}
				else {																			// ������ ������ ���� ������ �ƴ� �Ϲ� ������ ���

					int len = recv(read.fd_array[i], buf, sizeof(buf), 0);						// �б�
					if (len == 0) {
						FD_CLR(read.fd_array[i], &read);
						printf("[%d]���� ���������ϼ̽��ϴ�.\n", read.fd_array[i]);
						closesocket(temp.fd_array[i]);
					}
					else {
						buf[len] = 0;
						printf("[MAIN]recv message :				%s<%d>\n", buf, read.fd_array[i]);
						if (buf[0] == '$') {
							sscanf(buf, "$%d", &target_id);
							target_port = cmd_ctr.Find_Client(target_id);
							if (target_port == -1)
								continue;

							sprintf(buf, "@PORT:%d\n", target_port);

							send_t[thread_temp++] = thread(OJJJ_Thread_Send, target_port, cmd_ctr.Get_index(target_id));

							send(read.fd_array[i], buf, strlen(buf), 0);
							continue;
						}
						else if (buf[0] == '^') {
							char user_name[20];
							sscanf(buf, "^%s", user_name);
							cmd_ctr.Input_name(user_name, read.fd_array[i]);
							continue;
						}
						cout << "[MAIN] send message :					" << buf << endl;
						sprintf(msg, "[%s]%s", cmd_ctr.Get_Name(read.fd_array[i]), buf);
						send(read.fd_array[i], msg, strlen(msg), 0);
					}
				}
			}
		}
	}
	for (int i = 0; i < 5; i++)
		t[i].join();

}

// �̹��� ���� ������
void OJJJ_Thread_Recv(int port, int index) {
	printf("[RECEIVER] Thread START<%d>\n", port);
	USER* user = &cmd_ctr.Get_user(index);
	SOCKET sock = Serv_Initialize(port, 2);
	Mat rawData;
	Mat frame;
	char* longbuf;
	char buffer[BUF_LEN]; // Buffer for echo string
	int recvMsgSize; // Size of received message
	//clock_t last_cycle = clock();
	int total_pack;
	while (1) {
		// Block until receive message from a client
		do {
			// Ŭ���̾�Ʈ�� ���� �� ���۹��� ��Ŷ���� ��������
			recvMsgSize = recv(sock, buffer, BUF_LEN, 0);
		} while (recvMsgSize > sizeof(int));
		// ��Ŷ �� ����
		total_pack = ((int*)buffer)[0];

		//cout << "expecting length of packs:" << total_pack << endl;
		// ��Ŷ�� ũ��� ����ŭ ���� ����
		longbuf = new char[PACK_SIZE * total_pack];

		// ��Ŷ�� �� ��ŭ ������ ���ú�
		for (int i = 0; i < total_pack; i++) {
			recvMsgSize = recv(sock, buffer, BUF_LEN, 0);
			// ��Ŷ ������� ���� �����͸� ���Ž� ����
			if (recvMsgSize != PACK_SIZE) {
				//cerr << "Received unexpected size pack:" << recvMsgSize << endl;
				continue;
			}
			// �����ؼ� ���ú��� �����͸� �ϳ��� ������ ����
			memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
		}

		// ��ü������ ���ú��� �����͸� �̹��� �����ͷ� ��ȯ
		rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
		frame = imdecode(rawData, IMREAD_COLOR);

		if (frame.size().width == 0) {
			cerr << "decode failure!" << endl;
			continue;
		}

		//imshow("recv", frame);
		waitKey(1);


		user->lock.write_lock();
		//cout << "write_locking _ " << port << endl;
		//RW.write_lock();
		memcpy(&MATS[index], &rawData, sizeof(Mat));
		//RW.write_unlock();
		user->lock.write_unlock();
		//cout << "write_unlocking _ " << port << endl;
		isSAVE[index] = true;
		free(longbuf);
	}
	closesocket(sock);
	printf("[RECEIVER] Thread END<%d>\n", port);
}

// �̹��� �۽� ������
void OJJJ_Thread_Send(int port, int index) {
	printf("[SENDER] Thread START<%d / %d>\n", port, index);
	USER* user = &cmd_ctr.Get_user(index);
	SOCKET sock = Serv_Initialize(port, 2);
	SOCKADDR_IN target_adr;
	memset(&target_adr, 0, sizeof(target_adr));

	char msg[BUF_SIZE];
	int msg_len;
	int client_adr_sz;
	client_adr_sz = sizeof(target_adr);
	msg_len = recvfrom(sock, msg, BUF_SIZE, 0, (struct sockaddr*) & target_adr, &client_adr_sz);
	msg[msg_len] = 0;
	printf("[SENDER]recv message :				%s\n", msg);
	
	printf("[SENDER] waiting connect!\n");
	connect(sock, (struct sockaddr*) & target_adr, sizeof(target_adr));
	printf("[SENDER] connect done!\n");

	int jpegqual = ENCODE_QUALITY;
	Mat temp;
	Mat frame;
	vector<uchar>encoded;
	vector < int > compression_params;
	int ibuf[1];
	int total_pack;

	while (1) {
		if (isSAVE[index] == false) {
			//cout << "img does not saved["<<index<<"]"<<endl;
			continue;
		}
		try {
			//printf("[SENDER] read from :			%d\n",index);

			user->lock.read_lock();
			//cout << "read_locking _ " << port << endl;
			//RW.read_lock();
			memcpy(&temp, &MATS[index], sizeof(Mat));
			user->lock.read_unlock();
			//cout << "read_unlocking _ " << port << endl;
			//RW.read_unlock();

			frame = imdecode(temp, IMREAD_COLOR);
			if (frame.size().width == 0) {
				//cerr << "decode fadilure!" << endl;
				continue;
			}

			// ���
			/*printf("i can print : %d\n", frame.size().width);
			imshow("recv", frame);
			waitKey(1);*/

			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);
			// ������ ��ȯ
			imencode(".jpg", frame, encoded, compression_params);
			//imshow("send", temp);
			// ���۵� ��Ŷ�� ����5 ����
			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;


			ibuf[0] = total_pack;

			// ���ۿ� �ռ� ��Ŷ �� ����
			send(sock, (char*)ibuf, sizeof(int), 0);
			//puts("packet size send");
			// ������ ��Ŷ �� ��ŭ ������ ����
			for (int i = 0; i < total_pack; i++) {
				send(sock, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0);
				//printf("send packet data %d\n", i);
			}
			waitKey(FRAME_INTERVAL);
		}
		catch (exception e) { 
			cout<<"catch somthing"<<endl;
			continue; }

	}
	printf("[SENDER] Thread END<%d>\n", port);
}