#include "_CLASS.h"

	
COMMAND_CENTER::COMMAND_CENTER(int port) {
	for (int i = 0; i < MAXIMUM_USER; i++) {
		users[i].input_data(-1, port + (i * 10) + 1);
	}
}

	// ���� ����� Ŭ���̾�Ʈ���� id�� ��Ʈ��ȣ�� �����ϰ� ������ �ϴ� �Լ�
int COMMAND_CENTER::Connect_New_Client(int client_id) {
	//int i = Get_blank_index();
	int i = Get_index();
	if (i == -1) {
		printf("������ �� ���� �Ҵ�\n");
		return -1;
	}

	users[i].set_id(client_id);

	char msg[BUF_SIZE];

	sprintf(msg, "#ID:%d,PORT:%d\n", client_id, users[i].get_port());
	send(client_id, msg, strlen(msg), 0);
	

	return users[i].get_port();
}

void COMMAND_CENTER::DisConnected_Client(int id) {
	int idx = Get_index(id);
	users[idx].release();
	
	printf("[%d]���� ���������ϼ̽��ϴ�.\n", id);
}

// index �� �ش� id�� ���Ե� index Ȥ�� �� �ε����� ã�� �Լ�
// -1 : �� �ε��� , �� �� ���� : �ش� ���̵�
int COMMAND_CENTER::Get_index(int client_id) {
	int i;
	for (i = 0; i < MAXIMUM_USER; i++)
		if (users[i].get_id() == client_id)
			break;

	if (i == MAXIMUM_USER)
		return -1;
	else
		return i;
}

// Ư�� Ŭ���̾�Ʈ�� ��Ʈ�� ã�� �Լ�
int COMMAND_CENTER::Get_port(int id) {
	if (users[Get_index(id)].get_id() == -1)
		return -1;

	return users[Get_index(id)].get_port(1);
}

// ���� ����� Ŭ���̾�Ʈ�� ����Ǿ��ִ� Ŭ���̾�Ʈ ���� �ĺ�
void COMMAND_CENTER::Check_Client(int target, int port) {
	char msg[BUF_SIZE];
	puts("[sysetm] client checking");
	for (int i = 0; i < MAXIMUM_USER; i++) {

		int temp_id=users[i].get_id();
		int temp_port = users[i].get_port();
		if (temp_id != -1 && temp_port != port) {
			sprintf(msg, "$NEW:%d,PORT:%d\n", target, port);
			send(users[i].get_id(), msg, strlen(msg), 0);
			sprintf(msg, "$NEW:%d,PORT:%d\n", temp_id, temp_port);
			send(target, msg, strlen(msg), 0);
		}
	}
}
void  COMMAND_CENTER::Input_name(char* name, int id) {
	int index = Get_index(id);
	users[index].set_name(name);
}

int COMMAND_CENTER::User_Num() {
	int sum = 0;
	for (int i = 0; i < MAXIMUM_USER; i++) {
		if (users[i].get_id() != -1)
			sum++;
	}
	return sum;
}


char* COMMAND_CENTER::Get_Name(int id) {
	int index = Get_index(id);
	char* temp = users[index].get_name();
	if (temp == NULL) {
		char def[] = "default";
		return def;
	}
	
	return temp;
}
USER COMMAND_CENTER::Get_user_idx(int index) {
	return users[index];
}

USER COMMAND_CENTER::Get_user_id(int id) {
	return users[Get_index(id)];
}