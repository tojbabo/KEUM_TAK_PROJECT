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

COMMAND_CENTER cmd_ctr;

thread t_ai;

// �̹��� ���� ������
void OJJJ_Thread_Recv(int port, int index) {
	printf("[RECEIVER] Thread START<%d>\n", port);
	USER* user = &cmd_ctr.Get_user(index);
	SOCKET sock = Initialize(port, 2);
	Mat rawData;
	Mat frame;

	char* longbuf;
	char buffer[BUF_LEN]; // Buffer for echo string
	int recvMsgSize; // Size of received message
	//clock_t last_cycle = clock();
	int total_pack;
	while (1) {
		// Block until receive message from a client
		try {
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
				memcpy(&longbuf[i * PACK_SIZE], buffer, recvMsgSize);
			}

			// ��ü������ ���ú��� �����͸� �̹��� �����ͷ� ��ȯ
			rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);

			frame = imdecode(rawData, IMREAD_COLOR);

			//if (frame.size().width == 0) {
			//	cerr << "decode failure!" << endl;
			//	continue;
			//}

			if (frame.size().width != 320 && frame.size().height != 240) {
				cout << "������ ������ Ʋ��" << endl;
				continue;
			}

			//imshow("recv", frame);			

			user->lock.write_lock();
			//cout << "write_locking _ " << port << endl;
			//RW.write_lock();
			memcpy(&MATS[index], &rawData, sizeof(Mat));
			//RW.write_unlock();
			user->lock.write_unlock();
			//cout << "write_unlocking _ " << port << endl;
			isSAVE[index] = true;
			free(longbuf); waitKey(1000 / 30);

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
			//printf("[SENDER] read from :			%d\n",index);

			user->lock.read_lock();
			//cout << "read_locking _ " << port << endl;
			//RW.read_lock();
			//memcpy(&temp, &MATS[index], sizeof(Mat));
			
			//cout << "read_unlocking _ " << port << endl;
			//RW.read_unlock();
			


			Mat frame = imdecode(MATS[index], IMREAD_COLOR);
			user->lock.read_unlock();
			if (frame.size().width == 0) {
				////cerr << "decode fadilure!" << endl;
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
			waitKey(1000/20);
		}
		catch (exception e) {
			cout << "catch somthing" << endl;
			continue;
		}

	}
	printf("[SENDER] Thread END<%d>\n", port);
}

void OJJJ_Thread_AI(int PORT) {
	// ���μ��� ����
	int id = (PORT - 9000 + 100) / 100;
	char Process_number[] = "00";
	char Control_file[10];
	int a, b;
	a = id / 10;
	b = id % 10;
	sprintf(Process_number, "%d%d", a, b);
	//sprintf(Control_file, "C%s", Process_number);
	sprintf(Control_file, "KEY1");
	HANDLE C_h = File_Mapping(Control_file);
	LPCTSTR C_Sbuf = Make_Shared_Memory(C_h);
	Mat temp;

	CopyMemory((PVOID)C_Sbuf, "3god", strlen("3god"));
	cout << "-------------------------control file open " << endl;

	vector<uchar>encoded;
	vector < int > compression_params;

	HANDLE h[MAXIMUM_USER];
	LPCTSTR Sbuf[MAXIMUM_USER];
	bool wasSAVE[MAXIMUM_USER];

	USER* user;

	for (int i = 0; i < MAXIMUM_USER; i++) {
		sprintf(Control_file, "I%s_%d", Process_number, i);
		h[i] = File_Mapping(Control_file);
		Sbuf[i] = Make_Shared_Memory(h[i]);
		wasSAVE[i] = false;
	}
	int size;
	char MSG[20];
	while (1) {
		if (isREAD) {
			for (int i = 0; i < MAXIMUM_USER; i++) {
				EMOTION[i] = -1;
			}
		}
		if (C_Sbuf[0] == '0') {
			char str[20];
			strncpy(str, (char*)C_Sbuf,20);
			cout << "EMOTION check! : " << str << endl;
			char* ptr = strtok(str, ",");
			ptr = strtok(NULL, ",");
			for (int i = 0;i<MAXIMUM_USER; i++) {
				if (ptr==NULL||ptr[0]=='-')
					break;
				cout << i << "'s emotion is : " << ptr << endl;
				sscanf(ptr, "%d", &EMOTION[i]);
				ptr = strtok(NULL, ",");
			}
			cout << "making false";
			CopyMemory((PVOID)C_Sbuf, "3", strlen("3"));
			isREAD = false;
		}
		else {
			//cout << "not 0" << endl;
			Sleep(1000);
			continue;
		}
		

		int i;
		for (i = 0; i < MAXIMUM_USER; i++) {
			if (wasSAVE[i] == false && isSAVE[i] == true) {
				//cout << "-------------------------something was changed" << endl;
				wasSAVE[i] = true;
				sprintf(MSG, "2,I%s_%d", Process_number, i);
				CopyMemory((PVOID)C_Sbuf, MSG, strlen(MSG));
				cout << "[shared memory] new client write" << endl;
				break;
			}
			else if (isSAVE[i]) {
				user = &cmd_ctr.Get_user(i);

				//cout << "-------------------------writing try" << endl;

				try {
					user->lock.read_lock();
					memcpy(&temp, &MATS[i], sizeof(Mat));
					user->lock.read_unlock();

					if (temp.size().width == 0)
						continue;

					size = 1 + (encoded.size() - 1);

					// ���� ����
					compression_params.push_back(IMWRITE_JPEG_QUALITY);
					compression_params.push_back(ENCODE_QUALITY);
					imencode(".jpg", temp, encoded, compression_params);

					CopyMemory((PVOID)Sbuf[i], &encoded[0], size);
					sprintf(MSG, "1,", Process_number, i);
					CopyMemory((PVOID)C_Sbuf, MSG, strlen(MSG));
					cout << "[share memroy] write image" << endl;

					waitKey(FRAME_INTERVAL);
				}
				catch (Exception e) { continue; }
			}
		}
		if (i == MAXIMUM_USER)
			Sleep(1000);
	}
	Shared_Clear(Sbuf, h, MAXIMUM_USER);
}

// TCP - SELECT ��� �Լ�
void LOGIC_chatting(int PORT){
	cout << endl << "----------------------------------" << endl;
	cout << "[F_CHAT.cpp] START<�ڽ�>" << endl; 

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
		if (re == 0) continue;

		

		for (int i = 0; i < read.fd_count; i++) {												// select ������ ��ŭ �ݺ�
			if (FD_ISSET(read.fd_array[i], &temp))												// Ư�� ������ �ӽ� �迭�� �����ϴ��� Ȯ��
			{
				/*
				if (!isREAD) {
					char temp_msg[5];
					cout << "�о� ���ô�." << endl;
					for (int j = 0; j < MAXIMUM_USER; j++) {
						if (EMOTION[j] != -1) {
							sprintf(temp_msg, "&,%d,%d\n", cmd_ctr.Get_user(j).get_id(), EMOTION[j]);
							cout << "���� �޽��� : " << temp_msg << endl;
							for (int k = 0; k < read.fd_count; k++) {
								cmd_ctr.Get_user(k).Send_Msg(temp_msg);
							}
						}
					}
					isREAD = true;
				}*/


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
						//printf("[%d]���� ���������ϼ̽��ϴ�.\n", read.fd_array[i]);
						// ������ ���� �����
						//int index = cmd_ctr.Get_index(read.fd_array[i]);
						cmd_ctr.DisConnected_Client(read.fd_array[i]);
						//cmd_ctr.Get_user(index).get_thread(0)->join();
						//t[0].join();
						//cout << "thread �����";
						
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

}

