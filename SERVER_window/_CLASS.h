#ifndef CLASS
#define CLASS
#include "_HEADER.h"
#include "_UTILITY.h"

#define CANNOT_ACCESS_PASSWD -1
#define CANNOT_ACCESS_LIMIT -2

using namespace std;

class THREAD_ {
	thread t;
	int data[2];
public:
	THREAD_* link;
	THREAD_() { link = NULL; }
	thread* get_thread(int Client_ONE, int Client_TWO) {
		data[0] = Client_ONE;
		data[1] = Client_TWO;
		return &t; 
	}
};

class RW_LOCK {
	bool read;
	bool write;
public:
	RW_LOCK() {
		read = true;
		write = true;
	}
	void read_lock() {
		while (!read) { puts("read locking"); }
		write = false;
	}
	void read_unlock() {
		write = true;
	}
	void write_lock() {
		while (!write) { puts("write locking"); }
		write = false;
		read = false;
	}
	void write_unlock() {
		read = true;
		write = true;

	}
};

class USER {
	int id;
	int port;
	THREAD_* head;
	char name[20];
public:
	RW_LOCK lock;
	USER(int user_id = -1, int user_port = -1);
	// 유저 종료. 정보 초기화
	void release();
	// 유저의 빈 스레드 가져옴
	std::thread* get_thread(int OtherClient);
	// 스레드를 생성함
	void make_thread(THREAD_* thread);
	// 스레드 종료시킴 - 구현 필요
	void release_thread();
	// 유저의 아이디 반환
	int get_id() { return id; }
	// 유저 아이디 설정
	void set_id(int ID) { id = ID; }
	// 유저의 다음 포트 가져옴
	int get_port(int dx=0) { 
		port += dx;
		return port; }
	// 유저 포트 설정
	void set_port(int PORT) { port = PORT; }
	// 유저 기본 정보 입력
	void input_data(int user_id, int user_port);
	// 유저 이름 정보 가져옴
	char* get_name() { return name; }
	// 유저 이름 설정
	void set_name(char* user_name);
	// 유저에게 메시지 전달
	void Send_Msg(char* msg_to_user);
};

class COMMAND_CENTER {
protected:
	USER users[MAXIMUM_USER];
public:
	COMMAND_CENTER(int port = 9010);
	// 새로 연결된 클라이언트에게 id와 포트번호를 통지하고 저장을 하는 함수
	int Connect_New_Client(int new_client_id);
	// 클라이언트가 종료될 때
	void DisConnected_Client(int disconnect_id);
	// index 중 해당 id가 포함된 index를 찾아내는 함수 / -1 혹은 인자 없다면 빈 index 반환
	int Get_index(int target_client_id = -1);
	// 특정 클라이언트의 포트를 찾는 함수
	int Get_port(int target_client_id);
	// 새로 연결된 클라이언트와 연결되어있는 클라이언트 서로 식별
	void Check_Client(int new_client_id, int new_client_port);
	// 해당 유저의 이름 저장
	void Input_name(char* client_name, int client_id);
	// 유저 수 반환
	int User_Num();
	// 유저 이름 반환
	char* Get_Name(int client_id);
	// 유저 인덱스 혹은 아이디로 유저 객체 가져옴
	USER Get_user_idx(int Client_idx);
	USER Get_user_id(int Client_id);
};

class ROOM {
private:
	const int limit = 2;
	int room_port;
	int room_id, person_num;
	string passwd;
	string title;
public:
	int emo[7];

	ROOM();
	// 방 생성
	ROOM(int room_id, int the_number_of_perosn, string room_title, string room_passwd);
	// 방 아이디 반환
	int Get_ID();
	// 방 포트 반환
	int Get_PORT();
	// 방의 지정된 제목 반환
	string Get_TITLE();
	// 방 정보를 하나의 스트링으로 얻어옴
	string Get_State();
	// 방에 접근
	int Enter(char* room_password);
	// 방에서 나감
	int Exit();
	// 해당 방 정보보기
	void Show();
};

class CORE {
	list<ROOM> RoomList;
	bool flag = false;
public:
	void LOCK() { while (flag) { Sleep(200); cout << " core - 락 걸림" << endl; } flag = true; }
	void UNLOCK() { flag = false; }


	// 방에 설정될 아이디 생성
	int Create_RoomID();
	// 모든 방에 대한 정보 소켓으로 전달
	void Response_List(SOCKET sock);
	// 방 생성
	ROOM Create_Room(char* RequestMessage);
	// 방에 접근 가능한지 여부 체크
	void IsEnterRoom(SOCKET sock, char* str);
	// 해당 방 탐색
	list<ROOM>::iterator Search_Room(int Room_ID);
	// 방 삭제
	void Delete_Room(int Room_ID);
	// 방에 대한 정보 소켓으로 알림
	void Notify_Enter(SOCKET sock, ROOM room);
};
#endif