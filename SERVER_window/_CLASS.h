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
	// ���� ����� Ŭ���̾�Ʈ���� id�� ��Ʈ��ȣ�� �����ϰ� ������ �ϴ� �Լ�
	int Connect_New_Client(int new_client_id);
	// Ŭ���̾�Ʈ�� ����� ��
	void DisConnected_Client(int disconnect_id);
	// index �� ����ִ� index�� ���� �Լ�
	int Get_blank_index();
	// index �� �ش� id�� ���Ե� index�� ã�Ƴ��� �Լ�
	int Get_index(int target_client_id);
	// Ư�� Ŭ���̾�Ʈ�� ��Ʈ�� ã�� �Լ�
	int Get_client_port(int target_client_id);
	int Find_Client(int target_client_id);
	// ���� ����� Ŭ���̾�Ʈ�� ����Ǿ��ִ� Ŭ���̾�Ʈ ���� �ĺ�
	void Check_Client(int new_client_id, int new_client_port);

	// ������Ʈ ���� ����
	void Input_name(char* client_name, int client_id);
	// ������Ʈ �� USER ������Ʈ�� ���� 
	void input_name_(char* client_name, int client_id);
	// �� �� �ϳ��� �Ƚ� �ھƾߵ�

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
	// �� ����
	ROOM(int room_id, int the_number_of_perosn, string room_title, string room_passwd);

	int Get_ID();

	int Get_PORT();
	
	string Get_TITLE();

	// �� ������ �ϳ��� ��Ʈ������ ����
	string Get_State();

	// �濡 ����
	int Enter(char* room_password);

	// �ش� �� ��������
	void Show();

};
