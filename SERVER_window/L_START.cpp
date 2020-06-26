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

// 방 목록 전달 함수
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
		// 사람 꽉참
		_System_Msg_Send(sock, "POOLBANG.");
	}
	else if (port == 0) {
		// 비번 틀림
		_System_Msg_Send(sock, "Not fit Password.");
	}
	else {
		// 포트 번호 부여
		//_System_Msg_Send(sock, "Enter the room.");
		char msg[BUF_SIZE];
		sprintf(msg, "#,[temp]title,%d\n", port);
		send(sock, msg, strlen(msg), 0);
	}
}

void _Hash_Task(SOCKET sock, string title,int port) {
	if (port == -1) {
		// 사람 꽉참
		_System_Msg_Send(sock, "POOLBANG.");
	}
	else if (port == 0) {
		// 비번 틀림
		_System_Msg_Send(sock, "Not fit Password.");
	}
	else {
		// 포트 번호 부여
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

// 방 생성 함수
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

	//감정 값 셋팅


	if (room_num > List_->back().Get_ID() || List_->size() == 0) {
		// 새로 발급된 아이디가 맨 마지막놈보다 뒤다 크다
		// 그럼 맨 뒤로
		cout << "최대 인덱스보다 크거나 최초의 노드임" << endl;
		temp = new ROOM(room_num, 0, title, passwd);
		List_->push_back(*temp);
		temp->Show();
	}
	else {
		list<ROOM>::iterator ptr;
		for (ptr = List_->begin(); ptr != List_->end(); ptr++) {
			// 리스트 내 모든 항목에 대해서
			if (ptr->Get_ID() < room_num) {
				cout << "번호가 더 낮음" << endl;
				continue;
			}
			else if (ptr->Get_ID() == room_num) {
				// 잘못된 경우 에러 반환하기 
				cout << "일치하는 번호 존재" << endl;
				continue;
			}
			else {
				cout << "생성중" << endl;
				temp = new ROOM(room_num, 0, title, passwd);
				List_->insert(ptr, *temp);
				temp->Show();
				break;
			}
		}
	}


	return temp;
}

// 목록 내 ID가 일치하는 방의 위치를 알아냄
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
		cout << "접근 성공";
		result= ptr->Get_PORT();
	}
	else if (num == 1) {
		cout << "비번 틀림";
		result= 0;
	}
	else if (num == 2) {
		cout << "사람 꽉참";
		result = -1;
	}

	_Hash_Task(sock, ptr->Get_TITLE(),result);
		
}

// 걍 기록용 사용 X 지워야댐 나중에
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
	cout << "[F_START.cpp] START<부모>" << endl;
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

	printf("대기방 서버 열고 기다리는 중 \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);

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
					printf("[start.cpp]New Connect user :			%d\n", client_sock);

					_Response_List(client_sock, list_room);
					
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
						printf("[start.cpp]recv message :				%s<%d>\n", buf, read.fd_array[i]);
						if (buf[0] == '!') {
							// !request - 리스트 요청
							_Response_List(read.fd_array[i], list_room);
						}
						else if (buf[0] == '@') {
							// @,TITLE,PASSWD - 방 생성
							int temp = _Create_ROOM_ID(list_room);
							ROOM* room =_Create_ROOM(&list_room, temp, buf);
							
							char child_port[10];
							sprintf(child_port, "%d", room->Get_PORT());
							
							cout << "자식 프로세스 생성 ~!!~!"<<endl;
							//cout << "int is : " << child_port << endl;
							//cout << "string is : " << temp << endl;
							ShellExecute(GetDesktopWindow(), _T("open"), _T("temp.exe"),child_port, 0, SW_SHOWDEFAULT);

							//_Hash_Task(read.fd_array[i], temp);
							_Hash_Task(read.fd_array[i], room->Get_TITLE(),room->Get_PORT());
						}
						else if (buf[0] = '#') {
							// #,ID,PASSWD  - 방 참여
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

