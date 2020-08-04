#include "_UTILITY.h"
#include "_CLASS.h"
#include "_LOGIC.h"
#include "_SHARED.h"

using namespace cv;
using namespace std;

Mat MATS[MAXIMUM_USER];
int isSAVE[MAXIMUM_USER] = { NOT_SAVED };

bool isREAD = true;
int EMOTION[MAXIMUM_USER];

COMMAND_CENTER* cmd_ctr;

void Thread_Recv_TCP(int port, int index) {
#pragma region ���� �ʱ�ȭ

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

	while (1) {
#pragma region ���
		
		try {
			do {
				recvMsgSize = recv(sock_, buffer, BUF_LEN, 0);

				if (recvMsgSize > 20) {
					continue;
				}
				else if (recvMsgSize == -1 || recvMsgSize == 0) {
					cout << "[RECEIVER] Disconnected -> end" << endl;
					isSAVE[index] = INTERRUPT;
					return;
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
					cout << "[RECEIVER] Disconnected" << endl;
					isSAVE[index] = INTERRUPT;
					free(longbuf);
					return;
				}
				else if (max_sz + recvMsgSize < img_sz) {
					//cout << "������ ��� �޴� ��" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					max_sz += recvMsgSize;
				}
				else if (max_sz + recvMsgSize == img_sz) {
					//cout << "��� ������ ����" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					react = false;
					break;

				}
				else {
					//cout << "������ �Ѿ���ϴ�. �ٽ� �����ϰڽ��ϴ�." << endl;
					react = true;
					free(longbuf);
					break;
				}
			}

			if (react) continue;

#pragma endregion

#pragma region ������ ��ȯ - ����
			
			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			frame = imdecode(rawData, IMREAD_COLOR);

			if (frame.size().width != 320 && frame.size().height != 240) {
				//cout << "������ ������ Ʋ��" << endl;
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
#pragma region ���� �ʱ�ȭ

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

	while (1) {
#pragma region ���

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

			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(ENCODE_QUALITY);

			imencode(".jpg", temp, encoded, compression_params);

			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;

			ibuf[0] = total_pack;

			// ���ۿ� �ռ� ��Ŷ �� ����
			msg_len = send(sock_, (char*)ibuf, sizeof(int), 0);
			if (msg_len == 0 || msg_len == -1) {
				cout << "[SENDER] Disconnected" << endl;
				return;
			}

			//puts("packet size send");
			// ������ ��Ŷ �� ��ŭ ������ ����
			for (int i = 0; i < total_pack; i++) {
				msg_len = send(sock_, (char*)&encoded[i * PACK_SIZE], PACK_SIZE, 0);
				if (msg_len == 0 || msg_len == -1) {
					cout << "[SENDER] Disconnected" << endl;
					return;
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

	int id = (PORT - 9000 + 100) / 100;
	char Process_number[] = "00";
	char Control_file[10];
	int a, b;
	a = id / 10;
	b = id % 10;


	sprintf(Process_number, "%d%d", a, b);																// ���μ��� �̸� ����
	//sprintf(Control_file, "C%s", Process_number);
	sprintf(Control_file, "KEY1");																		// �����޸� - �������� �̸�

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
		sprintf(Control_file, "I%s_%d", Process_number, i);												// ����� �Ҵ� ���� [ ���μ��� �̸�_����� �ε��� ]
		h[i] = File_Mapping(Control_file);
		Sbuf[i] = Make_Shared_Memory(h[i]);
		wasSAVE[i] = false;
	}

	int size;
	char MSG[20];

	while (1) {
		
		char str[20];
		strncpy(str, (char*)C_Sbuf, 20);
		if (isREAD) {																					// ������ �о�� �ϴ� ���
			for (int i = 0; i < MAXIMUM_USER; i++) {
				EMOTION[i] = -1;																		// ��� ������ -1(�⺻ ������)�� ����
			}
		}
		//cout << str << endl;

		// 1 - ���� �޸𸮿� �̹����� ������
		// 2 - ���ο� ����ڰ� �������� �˷�����
		// 3 - ���
		// 0 - ���� ����� ��������
		if (C_Sbuf[0] == '0') {

			char* ptr = strtok(str, ",");																// ���ڿ�����
			ptr = strtok(NULL, ",");																	// ��������
			for (int i = 0; i < MAXIMUM_USER; i++) {														// �Ľ� �ؿͼ�
				if (ptr == NULL || ptr[0] == '-')																// �迭�� ����
					break;

				//cout << cmd_ctr.Get_user(i).get_id()<< " is : " << ptr << endl;
				sscanf(ptr, "%d", &EMOTION[i]);
				ptr = strtok(NULL, ",");
			}
			CopyMemory((PVOID)C_Sbuf, "3", strlen("3"));
			isREAD = false;																				// ���� �о��� ���� ����
		}
		else {
			Sleep(1000);
			//CopyMemory((PVOID)C_Sbuf, "0", strlen("0"));
			continue;
		}

		int i;
		for (i = 0; i < MAXIMUM_USER; i++) {
			//cout << "was save : " << wasSAVE[i] << " is save : " << isSAVE[i] << ".................." << i << endl;
			if (wasSAVE[i] == false && isSAVE[i] == SAVED) {											// �������� ���žȵ� �̹����� ���ŵ� ���
				wasSAVE[i] = true;																		// ���� �Ǿ������� ����
				sprintf(MSG, "2,I%s_%d", Process_number, i);											// AI���� ���ο� �޸� �ּ� �˸�
				CopyMemory((PVOID)C_Sbuf, MSG, strlen(MSG));
				//cout << "[shared memory] new client write" << endl;
				break;
			}
			else if (isSAVE[i] == SAVED) {																// ���ŵǴ� �̹����� ������ ���
				user = &cmd_ctr->Get_user_idx(i);
				try {
					user->lock.read_lock();
					memcpy(&temp, &MATS[i], sizeof(Mat));												// �̹��� ������ ������
					user->lock.read_unlock();

					size = 1 + (encoded.size() - 1);
					if (temp.size().width == 0) {
						//cout << "sz is small" << endl;
						//CopyMemory((PVOID)C_Sbuf, "0,1,2,-", strlen("0,1,2,-"));
						continue;
					}

					// ���� ����
					compression_params.push_back(IMWRITE_JPEG_QUALITY);
					compression_params.push_back(ENCODE_QUALITY);
					imencode(".jpg", temp, encoded, compression_params);

					CopyMemory((PVOID)Sbuf[i], &encoded[0], size);										// �����޸𸮿� �̹��� ������ ����



					CopyMemory((PVOID)C_Sbuf, "1", strlen("1"));										// �̹����� ��ٰ� �˸�
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
	Shared_Clear(Sbuf, h, MAXIMUM_USER);																// ���� �޸� ����
}

// TCP - SELECT ��� �Լ�
int LOGIC_chatting(int PORT,int* opt) {

#pragma region �⺻ ��¹� & ���� ���� �� �ʱ�ȭ
	cout << endl << "----------------------------------" << endl;
	cout << "[F_CHAT.cpp] START<�ڽ�>" << endl;
	cout << "[EVENT] ȭ�� : " << opt[0] << endl;
	cout << "[EVENT] ���� : " << opt[1] << endl;
	cout << "[EVENT] ���� : " << opt[2] << endl;
	cout << "[EVENT] �ູ : " << opt[3] << endl;
	cout << "[EVENT] �ڿ� : " << opt[4] << endl;
	cout << "[EVENT] ���� : " << opt[5] << endl;
	cout << "[EVENT] ��� : " << opt[6] << endl;

	//thread t_ai = thread(OJJJ_Thread_AI, PORT);

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
	int connected_client_port;

	int target_id;
	int target_port;

	printf("���� ���� ��ٸ��� �� \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);

	for (int i = 0; i < MAXIMUM_USER; i++)
		EMOTION[i] = -1;

#pragma endregion

	while (1) {
		temp = read;																			// �ӽ� �迭 ����
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re�� select ����
		if (re == SOCKET_ERROR) break;

#pragma region ���� �ν� ����� ����ڿ��� �˸��� �κ�
		
		if (re == 0) {																			// timeout -> emotion notify
			if (!isREAD) {																		// ������ ���� ���
				char temp_msg[5];
				for (int j = 0; j < MAXIMUM_USER; j++) {
					if (EMOTION[j] != -1&&cmd_ctr->Get_user_idx(j).get_id()!=-1) {					// ������ �νĵǰ� ����ڰ� ���ӵǾ� �ִ� ���
						if (opt[EMOTION[j]] == 1) {
							// * : ��������
							// Ư�� ������� ������
							// �� ������ �̺�Ʈ ( 1 : ���� )�� ���
							// ���� ���� ��Ŵ
							cmd_ctr->Get_user_idx(j).Send_Msg((char*)"*,1\n");
							continue;
						}

						
						sprintf(temp_msg, "&,%d,%d\n", 
							cmd_ctr->Get_user_idx(j).get_id(), EMOTION[j]);							// �νĵ� Ư�� ������� ������ ��ü ����ڿ��� �˸�
						for (int k = 0; k < read.fd_count; k++) {
							cmd_ctr->Get_user_idx(k).Send_Msg(temp_msg);
						}
					}
				}
				isREAD = true;
			}
		}

#pragma endregion

		for (int i = 0; i < read.fd_count; i++) {												// select ������ ��ŭ �ݺ�

			if (FD_ISSET(read.fd_array[i], &temp))												// Ư�� ������ �ӽ� �迭�� �����ϴ��� Ȯ��
			{
#pragma region ���ο� ������ �Ǵ� �κ�

				if (sock == read.fd_array[i]) {													// ������ ������ ���� ������ ���
					SOCKADDR_IN client_adr;
					int size = sizeof(client_adr);
					SOCKET client_sock = accept(sock, (SOCKADDR*)&client_adr, &size);			// ������ �߰� ����

					FD_SET(client_sock, &read);													// select �迭�� �ش� ���� ����
					printf("[MAIN]New Connect user :			%d\n", client_sock);
					connected_client_port = cmd_ctr->Connect_New_Client(client_sock);

					if (connected_client_port == -1)
						printf("�� �̻� Ŭ���̾�Ʈ�� ���� �� ����.\n");
					else {
						*cmd_ctr->Get_user_id(client_sock).get_thread(client_sock)
							= thread(Thread_Recv_TCP, connected_client_port, cmd_ctr->Get_index(client_sock));
					}
					cmd_ctr->Check_Client(client_sock, connected_client_port);
				}
#pragma endregion

				else {																			// ������ ������ ���� ������ �ƴ� �Ϲ� ������ ���
					int len = recv(read.fd_array[i], Socket_Buffer, sizeof(Socket_Buffer), 0);						// �б�

#pragma region ������� ������ ����� �κ�

					if (len == 0 || len == -1) {
						FD_CLR(read.fd_array[i], &read);
						// ������ ���� �����
						cmd_ctr->DisConnected_Client(read.fd_array[i]);

						if (cmd_ctr->User_Num() == 0) {
							cout << "��� ����ڰ� ����" << endl << "���μ��� �����" << endl;
							system("pause");
							return 999;
						}
						closesocket(temp.fd_array[i]);
					}

#pragma endregion

#pragma region ����� ���� ��û �κ�

					else {
						Socket_Buffer[len] = 0;
						printf("[MAIN]recv message :				%s<%d>\n", Socket_Buffer, read.fd_array[i]);
						if (Socket_Buffer[0] == '$') {													// �̹��� ���� ��û 

							sscanf(Socket_Buffer, "$%d", &target_id);
							target_port = cmd_ctr->Get_port(target_id);

							if (target_port == -1)
								continue;

							sprintf(Socket_Buffer, "@PORT:%d\n", target_port);

						//	*cmd_ctr->Get_user_id(read.fd_array[i]).get_thread(target_id)
						//		= thread(Thread_Send_TCP, target_port, cmd_ctr->Get_index(target_id));

							send(read.fd_array[i], Socket_Buffer, strlen(Socket_Buffer), 0);
							continue;
						}
						else if (Socket_Buffer[0] == '^') {												// ����� ���̵� ����
							char user_name[20];
							sscanf(Socket_Buffer, "^%s", user_name);
							cmd_ctr->Input_name(user_name, read.fd_array[i]);
							continue;
						}
					
						sprintf(Socket_Buffer, "[%s]%s", cmd_ctr->Get_Name(read.fd_array[i]), Socket_Buffer);
						for (int i = 1; i < read.fd_count; i++) {
							send(read.fd_array[i], Socket_Buffer, strlen(Socket_Buffer), 0);

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

