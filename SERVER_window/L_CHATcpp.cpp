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

// �̹��� ���� ������
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
				cout << "�߰� ������ \n";
			}


			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
			//MATS[index] = rawData;
			
			frame = imdecode(rawData, IMREAD_COLOR);
			
			if (frame.size().width != 320 && frame.size().height != 240) {
				//cout << "������ ������ Ʋ��" << endl;
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
					cout << "������ ��� �޴� ��" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					max_sz += recvMsgSize;
				}
				else if(max_sz + recvMsgSize == img_sz){
					cout << "��� ������ ����" << endl;
					memcpy(&longbuf[max_sz], buffer, recvMsgSize);
					react = false;
					break;

				}
				else {
					cout << "������ �Ѿ���ϴ�. �ٽ� �����ϰڽ��ϴ�." << endl;
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
				//cout << "������ ������ Ʋ��" << endl;
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
// �̹��� �۽� ������
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

			

			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);
			imencode(".jpg", frame, encoded, compression_params);
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

			// ���� ����
			compression_params.push_back(IMWRITE_JPEG_QUALITY);
			compression_params.push_back(jpegqual);

			imencode(".jpg", temp, encoded, compression_params);

			total_pack = 1 + (encoded.size() - 1) / PACK_SIZE;


			ibuf[0] = total_pack;

			// ���ۿ� �ռ� ��Ŷ �� ����
			send(sock_, (char*)ibuf, sizeof(int), 0);
			//puts("packet size send");
			// ������ ��Ŷ �� ��ŭ ������ ����
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
			if (wasSAVE[i] == false && isSAVE[i] == true) {												// �������� ���žȵ� �̹����� ���ŵ� ���
				wasSAVE[i] = true;																		// ���� �Ǿ������� ����
				sprintf(MSG, "2,I%s_%d", Process_number, i);											// AI���� ���ο� �޸� �ּ� �˸�
				CopyMemory((PVOID)C_Sbuf, MSG, strlen(MSG));
				//cout << "[shared memory] new client write" << endl;
				break;
			}
			else if (isSAVE[i]) {																		// ���ŵǴ� �̹����� ������ ���
				user = &cmd_ctr.Get_user(i);
				try {
					user->lock.read_lock();
					memcpy(&temp, &MATS[i], sizeof(Mat));												// �̹��� ������ ������
					user->lock.read_unlock();


					Mat frame = imdecode(temp, IMREAD_COLOR);

					size = 1 + (encoded.size() - 1);
					if (frame.size().width == 0) {
						//cout << "sz is small" << endl;
						//CopyMemory((PVOID)C_Sbuf, "0,1,2,-", strlen("0,1,2,-"));
						continue;
					}

					// ���� ����
					compression_params.push_back(IMWRITE_JPEG_QUALITY);
					compression_params.push_back(ENCODE_QUALITY);
					imencode(".jpg", frame, encoded, compression_params);

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
	cout << endl << "----------------------------------" << endl;
	cout << "[F_CHAT.cpp] START<�ڽ�>" << endl;
	cout << "[EVENT] ȭ�� : " << opt[0] << endl;
	cout << "[EVENT] ���� : " << opt[1] << endl;
	cout << "[EVENT] ���� : " << opt[2] << endl;
	cout << "[EVENT] �ູ : " << opt[3] << endl;
	cout << "[EVENT] �ڿ� : " << opt[4] << endl;
	cout << "[EVENT] ���� : " << opt[5] << endl;
	cout << "[EVENT] ��� : " << opt[6] << endl;
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

	printf("���� ���� ��ٸ��� �� \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);

	for (int i = 0; i < MAXIMUM_USER; i++)
		EMOTION[i] = -1;

	while (1) {
		temp = read;																			// �ӽ� �迭 ����
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re�� select ����
		if (re == SOCKET_ERROR) break;


		if (re == 0) {
			if (!isREAD) {																		// ������ ���� ���
				char temp_msg[5];
				for (int j = 0; j < MAXIMUM_USER; j++) {
					if (EMOTION[j] != -1&&cmd_ctr.Get_user(j).get_id()!=-1) {					// ������ �νĵǰ� ����ڰ� ���ӵǾ� �ִ� ���
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
						t[temp] = thread(recvthread, connected_client_port, cmd_ctr.Get_index(client_sock));
					}
					cmd_ctr.Check_Client(client_sock, connected_client_port);
				}

				else {																			// ������ ������ ���� ������ �ƴ� �Ϲ� ������ ���

					int len = recv(read.fd_array[i], buf, sizeof(buf), 0);						// �б�
					if (len == -1)
						continue;

					else if (len == 0) {
						FD_CLR(read.fd_array[i], &read);
						//printf("[%d]���� ���������ϼ̽��ϴ�.\n", read.fd_array[i]);
						// ������ ���� �����
						//int index = cmd_ctr.Get_index(read.fd_array[i]);
						cmd_ctr.DisConnected_Client(read.fd_array[i]);
						//cmd_ctr.Get_user(index).get_thread(0)->join();
						//t[0].join();
						//cout << "thread �����";
						if (cmd_ctr.User_Num() == 0) {
							cout << "��� ����ڰ� ����" << endl << "���μ��� �����" << endl;
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

