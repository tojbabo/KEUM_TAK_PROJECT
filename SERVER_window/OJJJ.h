#pragma once
#include<WinSock2.h>
#include <opencv2/opencv.hpp>
#include <iostream>          // For cout and cerr
#include <cstdlib>           // For atoi()
#include <thread>
#include <string.h>
#define FRAME_HEIGHT 240
#define FRAME_WIDTH 320
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

#define BUF_LEN 65540 // Larger than maximum UDP packet size
#define BUF_SIZE 1024

//#define IP "127.0.0.1"
#define IP "0.0.0.0"
//#define IP "220.149.128.87"
#define PORT 9000

#define MAXIMUM_USER 4

// ^ : Ŭ���̾�Ʈ�� �̸� ����
// $ : ���ο� ���ӿ� ���� ��ȣ - ���ο� ���� / ���� ����� Ŭ���̾�Ʈ�� ���� ��û
// # : �ڽ��� ���̵�, ��Ʈ�� �ο��ϴ� ��ȣ
// <%s> inet_ntoa(adr.sin_addr) - IP�ּ� , <%d> ntohs(adr.sin_port) - PORT ��ȣ

class THREAD_ {
	std::thread* t;
public:
	THREAD_* link;
	THREAD_() { link = NULL; t=NULL; }
	std::thread* get_thread() { return t; }
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
	USER() {
		head = new THREAD_;
		id = 0;
		port = 0;
	}
	std::thread* get_thread() {
		THREAD_* ptr = head;
		while (1) {
			if (ptr->link == NULL) {
				make_thread(ptr);				
				return ptr->get_thread();
			}
			else 
				ptr = ptr->link;
		}
	}
	void make_thread(THREAD_* p) {
		THREAD_* node;
		node = new THREAD_;
		p->link = node;
	}
	int get_id() { return id; }
	int get_port() { return ++port; }
	void input_data(int id_, int port_) {
		id = id_;
		port = port_;
	}
	void set_name(char* name_) {
		strcpy(name, name_);
	}

};
class COMMAND_CENTER {
protected:
	int ids[MAXIMUM_USER];
	int ports[MAXIMUM_USER];
	char Names[MAXIMUM_USER][20];

	USER users[MAXIMUM_USER];

public:
	COMMAND_CENTER() {
		for (int i = 0; i < MAXIMUM_USER; i++) {
			ids[i] = -1;
			ports[i] = 9010 + (i * 10);
		}
	}
	// ���� ����� Ŭ���̾�Ʈ���� id�� ��Ʈ��ȣ�� �����ϰ� ������ �ϴ� �Լ�
	int Connect_New_Client(int client_id) {
		int i = Get_blank_index();
		if (i == -1) {
			printf("������ �� ���� �Ҵ�\n");
			return -1;
		}
		char msg[BUF_SIZE];
		sprintf(msg, "#ID:%d,PORT:%d\n", client_id, ports[i]);
		send(client_id, msg, strlen(msg), 0);

		ids[i] = client_id;

		users[i].input_data(client_id, ports[i]);
		
		return ports[i];
	}
	// index �� ����ִ� index�� ���� �Լ�
	int Get_blank_index() {
		int i;
		for (i = 0; i < MAXIMUM_USER; i++)
			if (ids[i] == -1)
				break;
		if (i == MAXIMUM_USER)
			return -1;
		else
			return i;
	}
	// index �� �ش� id�� ���Ե� index�� ã�Ƴ��� �Լ�
	int Get_index(int client_id) {
		int i;
		for (i = 0; i < MAXIMUM_USER; i++)
			if (ids[i] == client_id)
				break;
		if (i == MAXIMUM_USER)
			return -1;
		else
			return i;
	}
	// Ư�� Ŭ���̾�Ʈ�� ��Ʈ�� ã�� �Լ�
	int Get_client_port(int id) {
		int temp;
		for (int i = 0; i < MAXIMUM_USER; i++) {
			temp = users[i].get_id();
			if (temp == 0)
				break;
			else if (id == temp)
				return users[i].get_port();
		}
		return -1;
		
	}
	int Find_Client(int id) {
		for (int i = 0; i < MAXIMUM_USER; i++) {
			if (id == ids[i])
				return ++ports[i];
		}
		return -1;
	}
	// ���� ����� Ŭ���̾�Ʈ�� ����Ǿ��ִ� Ŭ���̾�Ʈ ���� �ĺ�
	void Check_Client(int target, int port) {
		char msg[BUF_SIZE];
		puts("[sysetm] client checking");
		for (int i = 0; i < MAXIMUM_USER; i++) {
			if (ids[i] != -1 && ports[i] != port) {
				sprintf(msg, "$NEW:%d,PORT:%d\n", target, port);
				send(ids[i], msg, strlen(msg), 0);
				sprintf(msg, "$NEW:%d,PORT:%d\n", ids[i], ports[i]);
				send(target, msg, strlen(msg), 0);
			}
		}
	}
	void Input_name(char* name, int id) {
		int index = Get_index(id);
		strcpy(Names[index], name);
	}
	void input_name_(char* name, int id) {
		int index = Get_index(id);
		users[index].set_name(name);
	}
	char* Get_Name(int id) {
		int index = Get_index(id);

		if (Names[index] == NULL) {
			char def[] = "default";
			return def;
		}

		return Names[index];
	}

	USER Get_user(int index) {
		return users[index];
	}
};