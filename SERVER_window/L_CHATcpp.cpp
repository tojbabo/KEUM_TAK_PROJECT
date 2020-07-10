#include "_UTILITY.h"
#include "_CLASS.h"
#include "_LOGIC.h"
#include "_SHARED.h"

using namespace cv;
using namespace std;

//SOCKET Initialize(int, int);
//void Memset_(SOCKADDR_IN*, const char*, int);

bool isRECV;
bool isSEND;
bool isPRINT;

bool isREAD = true;
bool isSAVE[MAXIMUM_USER] = { false };
Mat MATS[MAXIMUM_USER];
int EMOTION[MAXIMUM_USER];

char Emotions[20];

char* imgptr;

COMMAND_CENTER cmd_ctr;

thread t_ai;
Mat ttt;

// 이미지 수신 스레드
void OJJJ_Thread_Recv(int port, int index) {
	printf("[RECEIVER] Thread START<%d>\n", port);
	USER* user = &cmd_ctr.Get_user(index);
	//SOCKET sock = Initialize(port, 2);
	SOCKET sock = Initialize(port, 1);
	Mat rawData;
	Mat frame;
	

	listen(sock, 1);
	SOCKADDR_IN adr;
	memset(&adr, 0, sizeof(adr));
	int adr_sz = sizeof(adr);
	
	SOCKET sock_ = accept(sock, (SOCKADDR*)&adr, &adr_sz);

	char* longbuf;
	char buffer[BUF_LEN]; 
	int recvMsgSize;
	int total_pack;
	int max_sz;

	while (1) {
		try {
			do {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
			} while (recvMsgSize > sizeof(int));
			total_pack = ((int*)buffer)[0];

			longbuf = new char[PACK_SIZE * total_pack];

			/*
			for (int i = 0; i < total_pack; i++) {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				if (recvMsgSize != PACK_SIZE) {
					//cerr << "Received unexpected size pack:" << recvMsgSize << endl;
					continue;
				}
				memcpy(&longbuf[i * PACK_SIZE], buffer, recvMsgSize);
			}*/

			max_sz = 0;
			while (1) {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				cout << "recv msg : <" << recvMsgSize << "/" << total_pack * PACK_SIZE << ">" << endl;
				memcpy(&longbuf[max_sz], buffer,recvMsgSize);

				max_sz += recvMsgSize;
				if (max_sz == total_pack * PACK_SIZE)
					break;
				cout << "추가 전송중 \n";
			}


			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			//MATS[index] = rawData;
			
			frame = imdecode(rawData, IMREAD_COLOR);
			
			if (frame.size().width != 320 && frame.size().height != 240) {
				//cout << "데이터 사이즈 틀림" << endl;
				continue;
			}
			
			user->lock.write_lock();
			memcpy(&MATS[index], &rawData, sizeof(Mat));
			user->lock.write_unlock();
			
			//ttt = frame;

			isSAVE[index] = true;
			free(longbuf); 
			//waitKey(1000 / 30);
		}
		catch (exception e) {
			cout << "recv thread error!" << endl;
		}
	}
	closesocket(sock);
	printf("[RECEIVER] Thread END<%d>\n", port);
}
void recvthread(int port, int index) {
	printf("[RECEIVER] Thread START<%d>\n", port);
	USER* user = &cmd_ctr.Get_user(index);
	//SOCKET sock = Initialize(port, 2);
	SOCKET sock = Initialize(port, 1);
	Mat rawData;
	Mat frame;

	listen(sock, 1);
	SOCKADDR_IN adr;
	memset(&adr, 0, sizeof(adr));
	int adr_sz = sizeof(adr);

	SOCKET sock_ = accept(sock, (SOCKADDR*)&adr, &adr_sz);

	char* longbuf;
	char buffer[BUF_LEN];
	int recvMsgSize;
	int total_pack;
	int max_sz;

	bool react = false;
	int img_sz;

	while (1) {
		try {
			do {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				if (recvMsgSize > 20) {
					continue;
				}
			} while (recvMsgSize > sizeof(int));
			total_pack = ((int*)buffer)[0];
			img_sz = PACK_SIZE * total_pack;

			longbuf = new char[img_sz];

			max_sz = 0;
			while (1) {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				cout << "recv msg : <" << recvMsgSize << "/" << img_sz << "> ...........["<<index<<"] ";

				if (max_sz + recvMsgSize < img_sz) {
					cout << "데이터 계속 받는 중" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					max_sz += recvMsgSize;
				}
				else if(max_sz + recvMsgSize == img_sz){
					cout << "모든 데이터 받음" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					react = false;
					break;

				}
				else {
					cout << "데이터 넘어갔습니다. 다시 시작하겠습니다." << endl;
					react = true;
					free(longbuf);
					break;
				}
			}
		
			if (react) continue;

			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			//MATS[index] = rawData;
			frame = imdecode(rawData, IMREAD_COLOR);

			if (frame.size().width != 320 && frame.size().height != 240) {
				//cout << "데이터 사이즈 틀림" << endl;
				continue;
			}
			//imshow("erer",frame);

			user->lock.write_lock();
			memcpy(&MATS[index], &frame, sizeof(Mat));
			user->lock.write_unlock();

			//ttt = frame;

			isSAVE[index] = true;

			free(longbuf);
			waitKey(1);
		}
		catch (exception e) {
			cout << "recv thread error!" << endl;
		}
	}
	closesocket(sock);
	printf("[RECEIVER] Thread END<%d>\n", port);
}
// 이미지 송신 스레드
void OJJJ_Thread_Send(int port, int index) {
	printf("[SENDER] Thread START<%d / %d>\n", port, index);
	USER* user = &cmd_ctr.Get_user(index);
	SOCKET sock = Initialize(port, 2);
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

			user->lock.read_lock();
			Mat frame = imdecode(MATS[index], IMREAD_COLOR);
			user->lock.read_unlock();

			if (frame.size().width == 0) {
				////cerr << "decode fadilure!" << endl;
				continue;
			}

			

			// 변수 설정
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);
			imencode(".jpg", frame, encoded, compression_params);
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
			waitKey(1000 / 20);
		}
		catch (exception e) {
			cout << "catch somthing" << endl;
			continue;
		}

	}
	printf("[SENDER] Thread END<%d>\n", port);
}
void sendThread(int port, int index) {
	printf("[SENDER] Thread START<%d / %d>\n", port, index);
	USER* user = &cmd_ctr.Get_user(index);
	SOCKET sock = Initialize(port, 1);
	SOCKADDR_IN target_adr;
	memset(&target_adr, 0, sizeof(target_adr));

	char msg[BUF_SIZE];
	int msg_len;
	int client_adr_sz;
	client_adr_sz = sizeof(target_adr);

	listen(sock, 1);
	SOCKET sock_ = accept(sock, (SOCKADDR*)&target_adr, &client_adr_sz);

	int jpegqual = ENCODE_QUALITY;
	Mat temp;

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

			user->lock.read_lock();
			memcpy(&temp, &MATS[index], sizeof(Mat));
			//Mat frame = imdecode(temp, IMREAD_COLOR);
			user->lock.read_unlock();
			
			if (temp.size().width != 320 && temp.size().height != 240) {
				cerr << "decode fadilure!" << endl;
				continue;
			}

			// 변수 설정
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);

			imencode(".jpg", temp, encoded, compression_params);

			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;


			ibuf[0] = total_pack;

			// 전송에 앞서 패킷 수 통지
			send(sock_, (char*)ibuf, sizeof(int), 0);
			//puts("packet size send");
			// 통지한 패킷 수 만큼 데이터 전송
			for (int i = 0; i < total_pack; i++) {
				send(sock_, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0);
				//printf("send packet data %d\n", i);
			}
			waitKey(1000 / 20);
		}
		catch (exception e) {
			cout << "catch somthing" << endl;
			continue;
		}

	}
	printf("[SENDER] Thread END<%d>\n", port);

}
void OJJJ_Thread_AI(int PORT) {

	int id = (PORT - 9000 + 100) / 100;
	char Process_number[] = "00";
	char Control_file[10];
	int a, b;
	a = id / 10;
	b = id % 10;


	sprintf(Process_number, "%d%d", a, b);																// 프로세스 이름 설정
	//sprintf(Control_file, "C%s", Process_number);
	sprintf(Control_file, "KEY1");																		// 공유메모리 - 제어파일 이름

	HANDLE C_h = File_Mapping(Control_file);
	LPCTSTR C_Sbuf = Make_Shared_Memory(C_h);
	Mat temp;


	CopyMemory((PVOID)C_Sbuf, "3god", strlen("3god"));
	//cout << "-------------------------control file open " << endl;

	vector<uchar>encoded;
	vector < int > compression_params;

	HANDLE h[MAXIMUM_USER];
	LPCTSTR Sbuf[MAXIMUM_USER];
	bool wasSAVE[MAXIMUM_USER];

	USER* user;

	for (int i = 0; i < MAXIMUM_USER; i++) {
		sprintf(Control_file, "I%s_%d", Process_number, i);												// 사용자 할당 파일 [ 프로세스 이름_사용자 인덱스 ]
		h[i] = File_Mapping(Control_file);
		Sbuf[i] = Make_Shared_Memory(h[i]);
		wasSAVE[i] = false;
	}

	int size;
	char MSG[20];

	while (1) {
		
		char str[20];
		strncpy(str, (char*)C_Sbuf, 20);
		if (isREAD) {																					// 감정을 읽어야 하는 경우
			for (int i = 0; i < MAXIMUM_USER; i++) {
				EMOTION[i] = -1;																		// 모든 감정을 -1(기본 데이터)로 변경
			}
		}
		//cout << str << endl;

		// 1 - 내가 메모리에 이미지를 썼을때
		// 2 - 새로운 사용자가 들어왔음을 알렸을때
		// 3 - 대기
		// 0 - 감정 결과가 나왔을때
		if (C_Sbuf[0] == '0') {

			char* ptr = strtok(str, ",");																// 문자열에서
			ptr = strtok(NULL, ",");																	// 감정상태
			for (int i = 0; i < MAXIMUM_USER; i++) {														// 파싱 해와서
				if (ptr == NULL || ptr[0] == '-')																// 배열에 저장
					break;

				//cout << cmd_ctr.Get_user(i).get_id()<< " is : " << ptr << endl;
				sscanf(ptr, "%d", &EMOTION[i]);
				ptr = strtok(NULL, ",");
			}
			CopyMemory((PVOID)C_Sbuf, "3", strlen("3"));
			isREAD = false;																				// 감정 읽었음 으로 변경
		}
		else {
			Sleep(1000);
			//CopyMemory((PVOID)C_Sbuf, "0", strlen("0"));
			continue;
		}

		int i;
		for (i = 0; i < MAXIMUM_USER; i++) {
			//cout << "was save : " << wasSAVE[i] << " is save : " << isSAVE[i] << ".................." << i << endl;
			if (wasSAVE[i] == false && isSAVE[i] == true) {												// 이전까지 수신안된 이미지가 수신된 경우
				wasSAVE[i] = true;																		// 수신 되었음으로 변경
				sprintf(MSG, "2,I%s_%d", Process_number, i);											// AI에게 새로운 메모리 주소 알림
				CopyMemory((PVOID)C_Sbuf, MSG, strlen(MSG));
				//cout << "[shared memory] new client write" << endl;
				break;
			}
			else if (isSAVE[i]) {																		// 수신되는 이미지가 존재할 경우
				user = &cmd_ctr.Get_user(i);
				try {
					user->lock.read_lock();
					memcpy(&temp, &MATS[i], sizeof(Mat));												// 이미지 데이터 가져옴
					user->lock.read_unlock();


					Mat frame = imdecode(temp, IMREAD_COLOR);

					size = 1 + (encoded.size() - 1);
					if (frame.size().width == 0) {
						//cout << "sz is small" << endl;
						//CopyMemory((PVOID)C_Sbuf, "0,1,2,-", strlen("0,1,2,-"));
						continue;
					}

					// 변수 설정
					compression_params.push_back(IMWRITE_JPEG_QUALITY);
					compression_params.push_back(ENCODE_QUALITY);
					imencode(".jpg", frame, encoded, compression_params);

					CopyMemory((PVOID)Sbuf[i], &encoded[0], size);										// 공유메모리에 이미지 데이터 적음



					CopyMemory((PVOID)C_Sbuf, "1", strlen("1"));										// 이미지를 썼다고 알림
					cout << "[share memroy] write image" << endl;

					waitKey(FRAME_INTERVAL);
				}
				catch (Exception e) {
					cout << "something was wrong" << endl;
					continue;
				}
			}

		}
		if (i == MAXIMUM_USER)
			Sleep(1000);
	}
	Shared_Clear(Sbuf, h, MAXIMUM_USER);																// 공유 메모리 정리
}
// TCP - SELECT 기능 함수
int LOGIC_chatting(int PORT,int* opt) {
	cout << endl << "----------------------------------" << endl;
	cout << "[F_CHAT.cpp] START<자식>" << endl;
	cout << "[EVENT] 화남 : " << opt[0] << endl;
	cout << "[EVENT] 역겸 : " << opt[1] << endl;
	cout << "[EVENT] 공포 : " << opt[2] << endl;
	cout << "[EVENT] 행복 : " << opt[3] << endl;
	cout << "[EVENT] 자연 : " << opt[4] << endl;
	cout << "[EVENT] 슬픔 : " << opt[5] << endl;
	cout << "[EVENT] 놀람 : " << opt[6] << endl;
	t_ai = thread(OJJJ_Thread_AI, PORT);


	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(0);

	SOCKET sock = Initialize(PORT, 1);
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

	printf("서버 열고 기다리는 중 \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);

	for (int i = 0; i < MAXIMUM_USER; i++)
		EMOTION[i] = -1;

	while (1) {
		temp = read;																			// 임시 배열 설정
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re에 select 설정
		if (re == SOCKET_ERROR) break;


		if (re == 0) {
			if (!isREAD) {																		// 감정을 읽은 경우
				char temp_msg[5];
				for (int j = 0; j < MAXIMUM_USER; j++) {
					if (EMOTION[j] != -1&&cmd_ctr.Get_user(j).get_id()!=-1) {					// 감정이 인식되고 사용자가 접속되어 있는 경우
						if (opt[EMOTION[j]] == 1) {
							cmd_ctr.Get_user(j).Send_Msg((char*)"*,1\n");
						}


						sprintf(temp_msg, "&,%d,%d\n", cmd_ctr.Get_user(j).get_id(), EMOTION[j]);
						for (int k = 0; k < read.fd_count; k++) {
							cmd_ctr.Get_user(k).Send_Msg(temp_msg);
						}
					}
				}
				isREAD = true;
			}
		}
		


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
						t[temp] = thread(recvthread, connected_client_port, cmd_ctr.Get_index(client_sock));
					}
					cmd_ctr.Check_Client(client_sock, connected_client_port);
				}

				else {																			// 반응된 소켓이 메인 소켓이 아닌 일반 소켓일 경우

					int len = recv(read.fd_array[i], buf, sizeof(buf), 0);						// 읽기
					if (len == -1)
						continue;

					else if (len == 0) {
						FD_CLR(read.fd_array[i], &read);
						//printf("[%d]님이 접속종료하셨습니다.\n", read.fd_array[i]);
						// 쓰레드 종료 만들기
						//int index = cmd_ctr.Get_index(read.fd_array[i]);
						cmd_ctr.DisConnected_Client(read.fd_array[i]);
						//cmd_ctr.Get_user(index).get_thread(0)->join();
						//t[0].join();
						//cout << "thread 종료됨";
						if (cmd_ctr.User_Num() == 0) {
							cout << "모든 사용자가 나감" << endl << "프로세스 종료됨" << endl;
							return 999;
						}

						closesocket(temp.fd_array[i]);
					}

					else {
						cout << "len is  : " << len<<endl;
						buf[len] = 0;
						printf("[MAIN]recv message :				%s<%d>\n", buf, read.fd_array[i]);
						if (buf[0] == '$') {
							sscanf(buf, "$%d", &target_id);
							target_port = cmd_ctr.Find_Client(target_id);
							if (target_port == -1)
								continue;

							sprintf(buf, "@PORT:%d\n", target_port);

							send_t[thread_temp++] = thread(sendThread, target_port, cmd_ctr.Get_index(target_id));

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
						for (int i = 1; i < read.fd_count; i++) {
							send(read.fd_array[i], msg, strlen(msg), 0);
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < 5; i++)
		t[i].join();
	t_ai.join();

	return -1;
}

