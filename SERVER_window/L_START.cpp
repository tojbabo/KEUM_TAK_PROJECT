#include "_LOGIC.h"
#include "_UTILITY.h"
#include "_CLASS.h"
#include<tchar.h>



using namespace cv;
using namespace std;

void DEBUG_ListShow(list<ROOM> List) {
	list<ROOM>::iterator ptr;

	for (ptr = List.begin(); ptr != List.end(); ptr++) {
		ptr->Show();
		
	}
}

void _System_Msg_Send(SOCKET sock, const char* str) {
	char msg[BUF_SIZE];
	sprintf(msg, "$,System Message,%s\n", str);
	send(sock, msg, strlen(msg), 0);
}

// �� ��� ���� �Լ�
void _Response_List(SOCKET sock, list<ROOM> List_) {
	list<ROOM>::iterator ptr;
	string total = "";
	string temp;
	const char* buf;
	for (ptr = List_.begin(); ptr != List_.end(); ptr++) {
		temp = ptr->Get_State();
		total += temp;
	}
	buf = total.c_str();

	cout <<"buf is : "<< buf << endl;
	send(sock, buf, strlen(buf), 0);
}

void _Hash_Task(SOCKET sock,int port) {
	if (port == -1) {
		// ��� ����
		_System_Msg_Send(sock, "POOLBANG.");
	}
	else if (port == 0) {
		// ��� Ʋ��
		_System_Msg_Send(sock, "Not fit Password.");
	}
	else {
		// ��Ʈ ��ȣ �ο�
		//_System_Msg_Send(sock, "Enter the room.");
		char msg[BUF_SIZE];
		sprintf(msg, "#,[temp]title,%d\n", port);
		send(sock, msg, strlen(msg), 0);
	}
}

void _Hash_Task(SOCKET sock, string title,int port) {
	if (port == -1) {
		// ��� ����
		_System_Msg_Send(sock, "POOLBANG.");
	}
	else if (port == 0) {
		// ��� Ʋ��
		_System_Msg_Send(sock, "Not fit Password.");
	}
	else {
		// ��Ʈ ��ȣ �ο�
		//_System_Msg_Send(sock, "Enter the room.");
		char msg[BUF_SIZE];
		sprintf(msg, "#,%s,%d\n",title ,port);
		send(sock, msg, strlen(msg), 0);
	}
}

int _Create_ROOM_ID(list<ROOM> List_) {
	if (List_.size() == 0)
		return 1;

	int num=1;
	list<ROOM>::iterator ptr;

	for (ptr = List_.begin(); ptr != List_.end(); ptr++) {
		if (num != ptr->Get_ID())
			break;
		num++;
	}
	return num;
}

// �� ���� �Լ�
ROOM* _Create_ROOM(list<ROOM>* List_, int room_num, char* str) {
	char str_temp[15];
	char _title[100];
	char _passwd[100];
	ROOM* temp;
	
	strcpy(str_temp, str);


	char* ptr = strtok(str, ",");
	ptr = strtok(NULL, ",");
	sscanf(ptr, "%s", _title);
	cout << "str is : " << str << strlen(str) << endl;

	if (str_temp[strlen(str_temp) - 2] == ',') {
		cout << "pass is null" << endl;
		sprintf(_passwd, "");
	}
	else {
		ptr = strtok(NULL, ",");
		sscanf(ptr, "%s", _passwd);
	}
		
	String passwd(_passwd);
	String title(_title);

	cout << "password is  : " << passwd << endl;
	temp = new ROOM(room_num, 0, title, passwd);

	//���� �� ����


	if (room_num > List_->back().Get_ID() || List_->size() == 0) {
		// ���� �߱޵� ���̵� �� �������𺸴� �ڴ� ũ��
		// �׷� �� �ڷ�
		cout << "�ִ� �ε������� ũ�ų� ������ �����" << endl;
		temp = new ROOM(room_num, 0, title, passwd);
		List_->push_back(*temp);
		temp->Show();
	}
	else {
		list<ROOM>::iterator ptr;
		for (ptr = List_->begin(); ptr != List_->end(); ptr++) {
			// ����Ʈ �� ��� �׸� ���ؼ�
			if (ptr->Get_ID() < room_num) {
				cout << "��ȣ�� �� ����" << endl;
				continue;
			}
			else if (ptr->Get_ID() == room_num) {
				// �߸��� ��� ���� ��ȯ�ϱ� 
				cout << "��ġ�ϴ� ��ȣ ����" << endl;
				continue;
			}
			else {
				cout << "������" << endl;
				temp = new ROOM(room_num, 0, title, passwd);
				List_->insert(ptr, *temp);
				temp->Show();
				break;
			}
		}
	}


	return temp;
}

// ��� �� ID�� ��ġ�ϴ� ���� ��ġ�� �˾Ƴ�
int _Search_ROOM(list<ROOM> List_, int ID) {
	list<ROOM>::iterator ptr;
	int i = 0;
	for (ptr = List_.begin(); ptr != List_.end(); ptr++,i++) {
		if (ptr->Get_ID() == ID)
			return i;
	}
	return -1;
}

int _EnterRoom(SOCKET sock, list<ROOM> List_, char* str) {
	int id;
	char passwd[10];
	char str_temp[15];

	strcpy(str_temp,str);


	char* c_ptr = strtok(str, ",");
	c_ptr = strtok(NULL, ",");
	sscanf(c_ptr, "%d", &id);

	

	if (str_temp[strlen(str_temp) - 2] == ',') {
		cout << "pass is null" << endl;
		sprintf(passwd, "");
	}
	else{
		c_ptr = strtok(NULL, ",");
		sscanf(c_ptr, "%s", &passwd);
	}





	list<ROOM>::iterator ptr;
	ptr = List_.begin();

	int num = _Search_ROOM(List_, id);
	advance(ptr, num);

	num = ptr->Enter(passwd);
	int result;
	if (num == 0) {
		cout << "���� ����";
		result= ptr->Get_PORT();
	}
	else if (num == 1) {
		cout << "��� Ʋ��";
		result= 0;
	}
	else if (num == 2) {
		cout << "��� ����";
		result = -1;
	}

	_Hash_Task(sock, ptr->Get_TITLE(),result);
		
}

// �� ��Ͽ� ��� X �����ߴ� ���߿�
void checking() {
	list<int> LIST;
	list<int>::iterator it;

	LIST.push_front(3);
	LIST.push_front(5);
	LIST.push_front(6);
	for (it = LIST.begin(); it != LIST.end(); it++) {
		cout << *it << endl;
	}
}


void LOGIC_watting(int PORT) {

	cout << endl << "----------------------------------" << endl;
	cout << "[F_START.cpp] START<�θ�>" << endl;
	list<ROOM> list_room;


	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		Error_("WSAStartup Error in START.cpp");

	SOCKET sock = Initialize(PORT, 1);
	if (listen(sock, 5) == SOCKET_ERROR)
		Error_("SOCKET Listen Error in START.cpp");

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

	printf("���� ���� ���� ��ٸ��� �� \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);

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
					printf("[start.cpp]New Connect user :			%d\n", client_sock);

					_Response_List(client_sock, list_room);
					
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
						printf("[start.cpp]recv message :				%s<%d>\n", buf, read.fd_array[i]);
						if (buf[0] == '!') {
							// !request - ����Ʈ ��û
							_Response_List(read.fd_array[i], list_room);
						}
						else if (buf[0] == '@') {
							// @,TITLE,PASSWD - �� ����
							int temp = _Create_ROOM_ID(list_room);
							ROOM* room =_Create_ROOM(&list_room, temp, buf);
							
							char child_port[10];
							sprintf(child_port, "%d", room->Get_PORT());
							
							cout << "�ڽ� ���μ��� ���� ~!!~!"<<endl;
							//cout << "int is : " << child_port << endl;
							//cout << "string is : " << temp << endl;
							ShellExecute(GetDesktopWindow(), _T("open"), _T("temp.exe"),child_port, 0, SW_SHOWDEFAULT);

							//_Hash_Task(read.fd_array[i], temp);
							_Hash_Task(read.fd_array[i], room->Get_TITLE(),room->Get_PORT());
						}
						else if (buf[0] = '#') {
							// #,ID,PASSWD  - �� ����
							_EnterRoom(read.fd_array[i],list_room, buf);
							//_Hash_Task(read.fd_array[i], num);
							//_Hash_Task(read.fd_array[i],*room, num);
							
						}
						else {}
							// garvage data

						//cout << "[start.cpp] send message :					" << buf << endl;
						//send(read.fd_array[i], buf, strlen(buf), 0);
					}
				}
			}
		}
	}
}

