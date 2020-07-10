#pragma once
#include"_HEADER.h"


using namespace std;

class THREAD_ {
	thread* t;
public:
	THREAD_* link;
	THREAD_() { link = NULL; t = NULL; }
	thread* get_thread() { return t; }
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
	USER();
	std::thread* get_thread(int thread_index);
	void make_thread(THREAD_* thread);
	void release_thread();
	int get_id() { return id; }
	int get_port() { return ++port; }
	void input_data(int user_id, int user_port);
	void set_name(char* user_name);
	void Send_Msg(char* msg_to_user);
};

class COMMAND_CENTER {
protected:
	int ids[MAXIMUM_USER];
	int ports[MAXIMUM_USER];
	char Names[MAXIMUM_USER][20];
	USER users[MAXIMUM_USER];

public:
	COMMAND_CENTER();
	// 새로 연결된 클라이언트에게 id와 포트번호를 통지하고 저장을 하는 함수
	int Connect_New_Client(int new_client_id);
	// 클라이언트가 종료될 때
	void DisConnected_Client(int disconnect_id);
	// index 중 비어있는 index를 얻어내는 함수
	int Get_blank_index();
	// index 중 해당 id가 포함된 index를 찾아내는 함수
	int Get_index(int target_client_id);
	// 특정 클라이언트의 포트를 찾는 함수
	int Get_client_port(int target_client_id);
	int Find_Client(int target_client_id);
	// 새로 연결된 클라이언트와 연결되어있는 클라이언트 서로 식별
	void Check_Client(int new_client_id, int new_client_port);

	// 오브젝트 내에 저장
	void Input_name(char* client_name, int client_id);
	// 오브젝트 내 USER 오브젝트에 저장 
	void input_name_(char* client_name, int client_id);
	// 둘 중 하나로 픽스 박아야됨

	int User_Num();

	char* Get_Name(int client_id);
	USER Get_user(int client_index_no_id);
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

	int Get_ID();

	int Get_PORT();
	
	string Get_TITLE();

	// 방 정보를 하나의 스트링으로 얻어옴
	string Get_State();

	// 방에 접근
	int Enter(char* room_password);

	// 해당 방 정보보기
	void Show();

};
