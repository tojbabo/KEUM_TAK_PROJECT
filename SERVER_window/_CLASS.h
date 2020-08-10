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
	// ���� ����. ���� �ʱ�ȭ
	void release();
	// ������ �� ������ ������
	std::thread* get_thread(int OtherClient);
	// �����带 ������
	void make_thread(THREAD_* thread);
	// ������ �����Ŵ - ���� �ʿ�
	void release_thread();
	// ������ ���̵� ��ȯ
	int get_id() { return id; }
	// ���� ���̵� ����
	void set_id(int ID) { id = ID; }
	// ������ ���� ��Ʈ ������
	int get_port(int dx=0) { 
		port += dx;
		return port; }
	// ���� ��Ʈ ����
	void set_port(int PORT) { port = PORT; }
	// ���� �⺻ ���� �Է�
	void input_data(int user_id, int user_port);
	// ���� �̸� ���� ������
	char* get_name() { return name; }
	// ���� �̸� ����
	void set_name(char* user_name);
	// �������� �޽��� ����
	void Send_Msg(char* msg_to_user);
};

class COMMAND_CENTER {
protected:
	USER users[MAXIMUM_USER];
public:
	COMMAND_CENTER(int port = 9010);
	// ���� ����� Ŭ���̾�Ʈ���� id�� ��Ʈ��ȣ�� �����ϰ� ������ �ϴ� �Լ�
	int Connect_New_Client(int new_client_id);
	// Ŭ���̾�Ʈ�� ����� ��
	void DisConnected_Client(int disconnect_id);
	// index �� �ش� id�� ���Ե� index�� ã�Ƴ��� �Լ� / -1 Ȥ�� ���� ���ٸ� �� index ��ȯ
	int Get_index(int target_client_id = -1);
	// Ư�� Ŭ���̾�Ʈ�� ��Ʈ�� ã�� �Լ�
	int Get_port(int target_client_id);
	// ���� ����� Ŭ���̾�Ʈ�� ����Ǿ��ִ� Ŭ���̾�Ʈ ���� �ĺ�
	void Check_Client(int new_client_id, int new_client_port);
	// �ش� ������ �̸� ����
	void Input_name(char* client_name, int client_id);
	// ���� �� ��ȯ
	int User_Num();
	// ���� �̸� ��ȯ
	char* Get_Name(int client_id);
	// ���� �ε��� Ȥ�� ���̵�� ���� ��ü ������
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
	// �� ����
	ROOM(int room_id, int the_number_of_perosn, string room_title, string room_passwd);
	// �� ���̵� ��ȯ
	int Get_ID();
	// �� ��Ʈ ��ȯ
	int Get_PORT();
	// ���� ������ ���� ��ȯ
	string Get_TITLE();
	// �� ������ �ϳ��� ��Ʈ������ ����
	string Get_State();
	// �濡 ����
	int Enter(char* room_password);
	// �濡�� ����
	int Exit();
	// �ش� �� ��������
	void Show();
};

class CORE {
	list<ROOM> RoomList;
	bool flag = false;
public:
	void LOCK() { while (flag) { Sleep(200); cout << " core - �� �ɸ�" << endl; } flag = true; }
	void UNLOCK() { flag = false; }


	// �濡 ������ ���̵� ����
	int Create_RoomID();
	// ��� �濡 ���� ���� �������� ����
	void Response_List(SOCKET sock);
	// �� ����
	ROOM Create_Room(char* RequestMessage);
	// �濡 ���� �������� ���� üũ
	void IsEnterRoom(SOCKET sock, char* str);
	// �ش� �� Ž��
	list<ROOM>::iterator Search_Room(int Room_ID);
	// �� ����
	void Delete_Room(int Room_ID);
	// �濡 ���� ���� �������� �˸�
	void Notify_Enter(SOCKET sock, ROOM room);
};
#endif