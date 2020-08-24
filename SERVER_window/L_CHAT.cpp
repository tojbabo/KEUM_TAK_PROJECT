#include "_UTILITY.h"
#include "_CLASS.h"
#include "_LOGIC.h"
#include "_SHARED.h"
#include "_AI.h"

using namespace cv;
using namespace std;

Mat MATS[MAXIMUM_USER];
int isSAVE[MAXIMUM_USER] = { NOT_SAVED };
int EMOTICON[MAXIMUM_USER];

bool isREAD = true;
bool RUNNER = true;

COMMAND_CENTER* cmd_ctr;

void Thread_Recv_TCP(int port, int index) {
#pragma region 변수 초기화

	printf("[RECEIVER] Thread START<%d>\n", port);
	USER* user = &cmd_ctr->Get_user_idx(index);
	//SOCKET sock = Initialize(port, 2);
	SOCKET sock = Initialize_(port, 1);
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

#pragma endregion

	while (RUNNER) {
#pragma region 통신
		
		try {
			do {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);

				if (recvMsgSize > 20) {
					continue;
				}
				else if (recvMsgSize == -1 || recvMsgSize == 0) {
					cout << "[RECEIVER] Disconnected -> 패킷 사이즈 받는 중 끊어짐 발견 [size : " << recvMsgSize << "]" << endl;
					recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
					if (recvMsgSize == -1 || recvMsgSize == 0) {
						cout << "[RECEIVER] Disconnected -> 찐 종료 [size : " << recvMsgSize << "]" << endl;
						system("pause");
						isSAVE[index] = INTERRUPT;
						return;
					}
				}
			} while (recvMsgSize > sizeof(int));
			total_pack = ((int*)buffer)[0];
			img_sz = PACK_SIZE * total_pack;

			longbuf = new char[img_sz];

			max_sz = 0;
			while (1) {
				
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);
				//cout << "recv msg : <" << recvMsgSize << "/" << img_sz << "> ...........["<<index<<"] ";
				if (recvMsgSize == -1 || recvMsgSize == 0) {
					cout << "[RECEIVER] Disconnected -> 이미지 받는 중 끊어짐 발견 [size : "<<recvMsgSize<<"]" << endl;
					if (recvMsgSize == -1 || recvMsgSize == 0) {
						cout << "[RECEIVER] Disconnected -> 찐 종료 [size : " << recvMsgSize << "]" << endl;
						system("pause");
						isSAVE[index] = INTERRUPT;
						free(longbuf);
						return;
					}
				}
				else if (max_sz + recvMsgSize < img_sz) {
					//cout << "데이터 계속 받는 중" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					max_sz += recvMsgSize;
				}
				else if (max_sz + recvMsgSize == img_sz) {
					//cout << "모든 데이터 받음" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					react = false;
					break;

				}
				else {
					//cout << "데이터 넘어갔습니다. 다시 시작하겠습니다." << endl;
					react = true;
					free(longbuf);
					break;
				}
			}

			if (react) continue;

#pragma endregion

#pragma region 데이터 변환 - 저장
			
			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			frame = imdecode(rawData, IMREAD_COLOR);

			if (frame.size().width != 320 && frame.size().height != 240) {
				//cout << "데이터 사이즈 틀림" << endl;
				continue;
			}

			user->lock.write_lock();
			memcpy(&MATS[index], &frame, sizeof(Mat));
			user->lock.write_unlock();
		

			isSAVE[index] = SAVED;
			
			free(longbuf);
			waitKey(1);
		}

#pragma endregion
		catch (exception e) {
			cout << "recv thread error!" << endl;
		}
	}
	closesocket(sock);
	printf("[RECEIVER] Thread END<%d>\n", port);
}
void Thread_Send_TCP(int port, int index) {
#pragma region 변수 초기화

	printf("[SENDER] Thread START<%d / %d>\n", port, index);
	USER* user = &cmd_ctr->Get_user_idx(index);
	SOCKET sock = Initialize_(port, 1);
	SOCKADDR_IN target_adr;
	memset(&target_adr, 0, sizeof(target_adr));

	int msg_len;
	int client_adr_sz;
	client_adr_sz = sizeof(target_adr);
	listen(sock, 1);
	SOCKET sock_ = accept(sock, (SOCKADDR*)&target_adr, &client_adr_sz);

	Mat temp;
	vector<uchar>encoded;
	vector < int > compression_params;
	int ibuf[1];
	int total_pack; 

#pragma endregion

	while (RUNNER) {
#pragma region 통신

		if (isSAVE[index] == INTERRUPT) {
			cout << "[SENDER] DIsconnected" << endl;
			MATS[index].release();
			return;
		}
		else if (isSAVE[index] == NOT_SAVED) {
			//cout << "img does not saved["<<index<<"]"<<endl;
			continue;
		}

		try {
			user->lock.read_lock();
			memcpy(&temp, &MATS[index], sizeof(Mat));
			//Mat frame = imdecode(temp, IMREAD_COLOR);
			user->lock.read_unlock();

			if (temp.size().width != 320 && temp.size().height != 240) {
				//cerr << "decode fadilure!" << endl;
				continue;
			}

			// 변수 설정
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(ENCODE_QUALITY);

			imencode(".jpg", temp, encoded, compression_params);

			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

			ibuf[0] = total_pack;

			// 전송에 앞서 패킷 수 통지
			msg_len = send(sock_, (char*)ibuf, sizeof(int), 0);
			if (msg_len == 0 || msg_len == -1) {
				cout << "[SENDER] Disconnected" << endl;
				if (msg_len == 0 || msg_len == -1) {
					cout << "[SENDER ]찐 종료" << endl;

					return;
				}
			}

			//puts("packet size send");
			// 통지한 패킷 수 만큼 데이터 전송
			for (int i = 0; i < total_pack; i++) {
				msg_len = send(sock_, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0);
				if (msg_len == 0 || msg_len == -1) {
					cout << "[SENDER] Disconnected" << endl;
					if (msg_len == 0 || msg_len == -1) {
						cout << "[SENDER ]찐 종료" << endl;

						return;
					}
				}
				//printf("send packet data %d\n", i);
			}
			waitKey(1000 / 20);
		}
#pragma endregion
		catch (exception e) {
			cout << "catch somthing" << endl;
			continue;
		}
	}
	printf("[SENDER] Thread END<%d>\n", port);
}
void OJJJ_Thread_AI(int PORT) {


#pragma region 기본 셋팅 & 변수 선언

	int id = (PORT - 9000 + 100) / 100;
	char Process_number[] = "00";
	char Control_file[10];
	int a, b;
	a = id / 10;
	b = id % 10;


	sprintf(Process_number, "%d%d", a, b);																// 프로세스 이름 설정
	sprintf(Control_file, "C%s", Process_number);														// 공유메모리 - 제어파일 이름
	//sprintf(Control_file, "KEY1");								

	ShellExecute(GetDesktopWindow(), _T("open"), _T("AI.exe"), Control_file, 0, SW_SHOWDEFAULT);

	HANDLE C_h;
	LPCTSTR C_Sbuf = CreateMemory(&C_h, Control_file);

	Mat temp;
	vector<uchar>encoded;
	vector < int > compression_params;

	HANDLE h[MAXIMUM_USER];
	LPCTSTR Sbuf[MAXIMUM_USER];
	bool wasSAVE[MAXIMUM_USER];

	USER* user;

#pragma region 사용자 공유메모리 생성부

	for (int i = 0; i < MAXIMUM_USER; i++) {
		sprintf(Control_file, "I%s_%d", Process_number, i);												// 사용자 할당 파일 [ 프로세스 이름_사용자 인덱스 ]
		//cout << "사용자 기본 파일 선언 ["<<i<<"]";
		Sbuf[i] = CreateMemory(&h[i], Control_file);
		wasSAVE[i] = false;
	}

#pragma endregion

#pragma endregion

	CopyMemory((PVOID)C_Sbuf, "3start", strlen("3start"));

	int size;
	char MSG[20];

	while (RUNNER) {
		
		char str[20];
		strncpy(str, (char*)C_Sbuf, 20);
		/*
		if (isREAD) {																					// 감정을 읽어야 하는 경우
			for (int i = 0; i < MAXIMUM_USER; i++) {
				EMOTION[i] = -1;																		// 모든 감정을 -1(기본 데이터)로 변경
			}
		}
		*/
		// 1 - 내가 메모리에 이미지를 썼을때
		// 2 - 새로운 사용자가 들어왔음을 알렸을때
		// 3 - 대기
		// 0 - 감정 결과가 나왔을때
		// 9 - 종료하라고 알림
		if (C_Sbuf[0] == '0') {
			//cout << "공유메모리 - 0 [감정이 인식되어 처리]\n";
			//cout << "인식 결과 : " << C_Sbuf << endl;

			char* ptr = strtok(str, ",");																// 문자열에서
			ptr = strtok(NULL, ",");																	// 감정상태
			for (int i = 0; i < MAXIMUM_USER; i++) {														// 파싱 해와서
				if (ptr == NULL || ptr[0] == '-')																// 배열에 저장
					break;

				int temp;

				sscanf(ptr, "%d", &temp);
				
				if (temp == 8) temp = 7;
				EMOTICON[i] = temp;

				ptr = strtok(NULL, ",");
			}
			CopyMemory((PVOID)C_Sbuf, "3", strlen("3"));
			//cout << "공유메모리 트리거 3으로 변경\n";
			isREAD = false;																				// 감정 읽었음 으로 변경
		}
		else {
			//cout << "공유 메모리 - 0아님 [AI의 응답을 기다리는 중]\n";
			Sleep(1000);
			continue;
		}

		int i;
		for (i = 0; i < MAXIMUM_USER; i++) {
			//cout << "was save : " << wasSAVE[i] << " is save : " << isSAVE[i] << ".................." << i << endl;
			if (wasSAVE[i] == false && isSAVE[i] == SAVED) {											// 이전까지 수신안된 이미지가 수신된 경우
				wasSAVE[i] = true;																		// 수신 되었음으로 변경
				sprintf(MSG, "2,I%s_%d", Process_number, i);											// AI에게 새로운 메모리 주소 알림
				cout << "AI에게 사용자 인식 중 ..... " << MSG << " \n";
				CopyMemory((PVOID)C_Sbuf, MSG, strlen(MSG));
				//cout << "[shared memory] new client write" << endl;
				break;
			}
			else if (isSAVE[i] == SAVED) {																// 수신되는 이미지가 존재할 경우
				//printf("사용자 [%d]의 이미지가 존재. 인식을 요청함\n");
				user = &cmd_ctr->Get_user_idx(i);
				try {
					user->lock.read_lock();
					memcpy(&temp, &MATS[i], sizeof(Mat));												// 이미지 데이터 가져옴
					user->lock.read_unlock();

					size = 1 + (encoded.size() - 1);
					if (temp.size().width == 0) {
						continue;
					}

					// 변수 설정
					compression_params.push_back(IMWRITE_JPEG_QUALITY);
					compression_params.push_back(ENCODE_QUALITY);
					imencode(".jpg", temp, encoded, compression_params);

					CopyMemory((PVOID)Sbuf[i], &encoded[0], size);										// 공유메모리에 이미지 데이터 적음
					CopyMemory((PVOID)C_Sbuf, "1", strlen("1"));										// 이미지를 썼다고 알림

					//cout << "[share memroy] write image" << endl;

					waitKey(FRAME_INTERVAL);
				}
				catch (Exception e) {
					//cout << "something was wrong" << endl;
					continue;
				}
			}

		}

//		if (i == MAXIMUM_USER)
//			Sleep(1000);
	}
	
	// 공유 메모리 정리
	CopyMemory((PVOID)C_Sbuf, "9,", strlen("9,"));
	Shared_Clear(Sbuf, h, MAXIMUM_USER);																

	UnmapViewOfFile(C_Sbuf);
	CloseHandle(C_h);

}

// TCP - SELECT 기능 함수
int LOGIC_chatting(int PORT,int* EventOption) {

#pragma region 기본 출력문 & 변수 선언 및 초기화
	cout << endl << "----------------------------------" << endl;
	cout << "[F_CHAT.cpp] START<자식>" << endl;
	cout << "[EVENT] 화남 : " << EventOption[0] << endl;
	cout << "[EVENT] 역겸 : " << EventOption[1] << endl;
	cout << "[EVENT] 공포 : " << EventOption[2] << endl;
	cout << "[EVENT] 행복 : " << EventOption[3] << endl;
	cout << "[EVENT] 자연 : " << EventOption[4] << endl;
	cout << "[EVENT] 슬픔 : " << EventOption[5] << endl;
	cout << "[EVENT] 놀람 : " << EventOption[6] << endl;

	thread t_ai = thread(OJJJ_Thread_AI, PORT);

	cmd_ctr = new COMMAND_CENTER(PORT);

	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		exit(0);

	SOCKET sock = Initialize_(PORT, 1);
	if (listen(sock, 5) == SOCKET_ERROR) {
		cout << "listening error \n";
		exit(-1);
	}

	fd_set read, temp;
	TIMEVAL time;
	FD_ZERO(&read);
	FD_SET(sock, &read);

	int re;
	char Socket_Buffer[BUF_SIZE];
	char msg[BUF_SIZE];
	int connected_client_port;

	int target_id;
	int target_port;

	printf("서버 열고 기다리는 중 \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);

	for (int i = 0; i < MAXIMUM_USER; i++) {
		EMOTICON[i] = 7;
	}

#pragma endregion

	while (1) {
		temp = read;																			// 임시 배열 설정
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re에 select 설정
		if (re == SOCKET_ERROR) break;

#pragma region 감정 인식 결과를 사용자에게 알리는 부분
		
		if (re == 0) {																			// timeout -> emotion notify
			if (!isREAD) {																		// 감정을 읽은 경우
				char temp_msg[10];
				for (int j = 0; j < MAXIMUM_USER; j++) {
					if (cmd_ctr->Get_user_idx(j).get_id()!=-1) {					// 감정이 인식되고 사용자가 접속되어 있는 경우
						int emo = EMOTICON[j];
						printf("[%d]의 감정은 <%d>\n", j, emo);
						
						// 서버에서 처리할 수 있는 감정이벤트 - 감정이 인식될 때
						if (emo != 7) {

							if (EventOption[emo] == EVENT_KICK) {
								// * : 강제퇴장
								// 특정 사용자의 감정이
								// 방 설정된 이벤트 ( 1 : 강퇴 )일 경우
								// 강제 퇴장 시킴
								cmd_ctr->Get_user_idx(j).Send_Msg((char*)"*,1,나가\n");
								continue;
							}
						

						}		

						sprintf(temp_msg, "&,%d,%d\n", 
							cmd_ctr->Get_user_idx(j).get_id(), emo);							// 인식된 특정 사용자의 감정을 전체 사용자에게 알림
						for (int k = 0; k < read.fd_count; k++) {
							cmd_ctr->Get_user_idx(k).Send_Msg(temp_msg);
						}
					}
				}
				isREAD = true;
			}
		}

#pragma endregion

		for (int i = 0; i < read.fd_count; i++) {												// select 사이즈 만큼 반복

			if (FD_ISSET(read.fd_array[i], &temp))												// 특정 소켓이 임시 배열에 존재하는지 확인
			{
#pragma region 새로운 연결이 되는 부분

				if (sock == read.fd_array[i]) {													// 반응된 소켓이 메인 소켓일 경우
					SOCKADDR_IN client_adr;
					int size = sizeof(client_adr);
					SOCKET client_sock = accept(sock, (SOCKADDR*)&client_adr, &size);			// 소켓을 추가 생성

					FD_SET(client_sock, &read);													// select 배열에 해당 소켓 설정
					connected_client_port = cmd_ctr->Connect_New_Client(client_sock);
					printf("[MAIN]New Connect user :			%d.....[%d]\n", client_sock,cmd_ctr->User_Num());

					if (connected_client_port == -1)
						printf("더 이상 클라이언트를 받을 수 없다.\n");
					else {
						*cmd_ctr->Get_user_id(client_sock).get_thread(client_sock)
							= thread(Thread_Recv_TCP, connected_client_port, cmd_ctr->Get_index(client_sock));
					}
					cmd_ctr->Check_Client(client_sock, connected_client_port);
				}
#pragma endregion

				else {																			// 반응된 소켓이 메인 소켓이 아닌 일반 소켓일 경우
					int len = recv(read.fd_array[i], Socket_Buffer, sizeof(Socket_Buffer), 0);	// 읽기

#pragma region 사용자의 연결이 종료된 부분

					if (len == 0 || len == -1) {
						FD_CLR(read.fd_array[i], &read);
						// 쓰레드 종료 만들기
						system("pause");

						cmd_ctr->DisConnected_Client(read.fd_array[i]);
						printf("[MAIN]User Disconnected :            %d.....[%d]\n", read.fd_array[i], cmd_ctr->User_Num());

						if (cmd_ctr->User_Num() == 0) {
							cout << "모든 사용자가 나감" << endl << "프로세스 종료됨" << endl;
							RUNNER = false;
							Sleep(3000);
							return 999;
						}
						closesocket(temp.fd_array[i]);
					}

#pragma endregion

#pragma region 사용자 정상 요청 부분

					else {
						Socket_Buffer[len] = 0;
						printf("[MAIN]recv message :				%s<%d>\n", Socket_Buffer, read.fd_array[i]);
						if (Socket_Buffer[0] == '$') {													// 이미지 수신 요청 

							sscanf(Socket_Buffer, "$%d", &target_id);
							target_port = cmd_ctr->Get_port(target_id);

							if (target_port == -1)
								continue;

							sprintf(Socket_Buffer, "@PORT:%d\n", target_port);

							*cmd_ctr->Get_user_id(read.fd_array[i]).get_thread(target_id)
								= thread(Thread_Send_TCP, target_port, cmd_ctr->Get_index(target_id));

							send(read.fd_array[i], Socket_Buffer, strlen(Socket_Buffer), 0);
						}
						else if (Socket_Buffer[0] == '^') {												// 사용자 아이디 선언
							char user_name[20];
							sscanf(Socket_Buffer, "^%s", user_name);
							cmd_ctr->Input_name(user_name, read.fd_array[i]);
						}
						else {
							int emo = EMOTICON[cmd_ctr->Get_index(read.fd_array[i])];
							//int emo = EMOTICON[i - 1];
							//printf("[%d]의 감정은 <%d>이다.\n", i, emo);
							if (emo != 7) {
								if (EventOption[emo] == EVENT_CHATFILTER) {
									cout << "필터링 됩니다.\n";
									sprintf(msg, "[%s] %s\n", cmd_ctr->Get_Name(read.fd_array[i]), ChatFilter(strlen(Socket_Buffer)));
								}
							}
							else sprintf(msg, "[%s] %s", cmd_ctr->Get_Name(read.fd_array[i]), Socket_Buffer);

							for (int i = 1; i < read.fd_count; i++) {
								send(read.fd_array[i], msg, strlen(msg), 0);
							}
						}
					}

#pragma endregion

				}
			}
		}
	}

	//t_ai.join();

	return -1;
}

