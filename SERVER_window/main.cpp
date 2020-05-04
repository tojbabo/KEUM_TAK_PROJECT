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

// 설정 값에 따라서 생성한 SOCKET을 반환하는 함수
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

// 입력된 값에 따라서 주소함수 초기화 함수
void OJJJ_Memset(SOCKADDR_IN* adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}

// TCP - SELECT 기능 함수
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

	printf("서버 열고 기다리는 중 \n");
	while (1) {
		temp = read;																			// 임시 배열 설정
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re에 select 설정
		if (re == SOCKET_ERROR) break;
		if (re == 0) continue;

		for (int i = 0; i < read.fd_count; i++) {												// select 사이즈 만큼 반복
			if (FD_ISSET(read.fd_array[i], &temp))												// 특정 소켓이 임시 배열에 존재하는지 확인
			{
				if (sock == read.fd_array[i]) {													// 반응된 소켓이 메인 소켓일 경우
					SOCKADDR_IN client_adr;
					int size = sizeof(client_adr);
					SOCKET client_sock = accept(sock, (SOCKADDR*)&client_adr, &size);			// 소켓을 추가 생성

					FD_SET(client_sock, &read);													// select 배열에 해당 소켓 설정
					printf("[MAIN]New Connect user :			%d\n", client_sock);
					connected_client_port = cmd_ctr.Connect_New_Client(client_sock);
					if (connected_client_port == -1) 
						printf("더 이상 클라이언트를 받을 수 없다.\n");
					else {
						int temp = cmd_ctr.Get_blank_index();
						t[temp] = thread(OJJJ_Thread_Recv, connected_client_port, cmd_ctr.Get_index(client_sock));
					}
					cmd_ctr.Check_Client(client_sock, connected_client_port);
				}
				else {																			// 반응된 소켓이 메인 소켓이 아닌 일반 소켓일 경우

					int len = recv(read.fd_array[i], buf, sizeof(buf), 0);						// 읽기
					if (len == 0) {
						FD_CLR(read.fd_array[i], &read);
						printf("[%d]님이 접속종료하셨습니다.\n", read.fd_array[i]);
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

// 이미지 수신 스레드
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
			// 클라이언트로 부터 총 전송받을 패킷수를 통지받음
			recvMsgSize = recv(sock, buffer, BUF_LEN, 0);
		} while (recvMsgSize > sizeof(int));
		// 패킷 수 저장
		total_pack = ((int*)buffer)[0];

		//cout << "expecting length of packs:" << total_pack << endl;
		// 패킷의 크기와 수만큼 변수 생성
		longbuf = new char[PACK_SIZE * total_pack];

		// 패킷의 수 만큼 데이터 리시브
		for (int i = 0; i < total_pack; i++) {
			recvMsgSize = recv(sock, buffer, BUF_LEN, 0);
			// 패킷 사이즈보다 작은 데이터를 수신시 무시
			if (recvMsgSize != PACK_SIZE) {
				//cerr << "Received unexpected size pack:" << recvMsgSize << endl;
				continue;
			}
			// 연속해서 리시브한 데이터를 하나의 변수로 통합
			memcpy(&longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
		}

		// 전체적으로 리시브한 데이터를 이미지 데이터로 변환
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

// 이미지 송신 스레드
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

			// 출력
			/*printf("i can print : %d\n", frame.size().width);
			imshow("recv", frame);
			waitKey(1);*/

			// 변수 설정
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);
			// 데이터 변환
			imencode(".jpg", frame, encoded, compression_params);
			//imshow("send", temp);
			// 전송될 패킷의 숫자5 설정
			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;


			ibuf[0] = total_pack;

			// 전송에 앞서 패킷 수 통지
			send(sock, (char*)ibuf, sizeof(int), 0);
			//puts("packet size send");
			// 통지한 패킷 수 만큼 데이터 전송
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