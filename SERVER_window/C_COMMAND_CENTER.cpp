#include "_CLASS.h"


COMMAND_CENTER::COMMAND_CENTER() {
	for (int i = 0; i < MAXIMUM_USER; i++) {
		ids[i] = -1;
		ports[i] = 9010 + (i * 10);
	}
}
	// ���� ����� Ŭ���̾�Ʈ���� id�� ��Ʈ��ȣ�� �����ϰ� ������ �ϴ� �Լ�
int COMMAND_CENTER::Connect_New_Client(int client_id) {
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

void COMMAND_CENTER::DisConnected_Client(int id) {
	int idx = Get_index(id);
	ids[idx] = -1;
	printf("[%d]���� ���������ϼ̽��ϴ�.\n", id);
}

	// index �� ����ִ� index�� ���� �Լ�
int COMMAND_CENTER::Get_blank_index() {
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
int COMMAND_CENTER::Get_index(int client_id) {
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
int COMMAND_CENTER::Get_client_port(int id) {
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
int COMMAND_CENTER::Find_Client(int id) {
	for (int i = 0; i < MAXIMUM_USER; i++) {
		if (id == ids[i])
			return ++ports[i];
	}
	return -1;
}
	// ���� ����� Ŭ���̾�Ʈ�� ����Ǿ��ִ� Ŭ���̾�Ʈ ���� �ĺ�
void COMMAND_CENTER::Check_Client(int target, int port) {
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
void  COMMAND_CENTER::Input_name(char* name, int id) {
	int index = Get_index(id);
	strcpy(Names[index], name);
}
void  COMMAND_CENTER::input_name_(char* name, int id) {
	int index = Get_index(id);
	users[index].set_name(name);
}

int COMMAND_CENTER::User_Num() {
	int sum = 0;
	for (int i = 0; i < MAXIMUM_USER; i++) {
		if (ids[i] != -1)
			sum++;
	}
	return sum;
}


char* COMMAND_CENTER::Get_Name(int id) {
	int index = Get_index(id);

	if (Names[index] == NULL) {
		char def[] = "default";
		return def;
	}

	return Names[index];
}
USER COMMAND_CENTER::Get_user(int index) {
	return users[index];
}