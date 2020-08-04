#include "_CLASS.h"

	
COMMAND_CENTER::COMMAND_CENTER(int port) {
	for (int i = 0; i < MAXIMUM_USER; i++) {
		users[i].input_data(-1, port + (i * 10) + 1);
	}
}

	// 새로 연결된 클라이언트에게 id와 포트번호를 통지하고 저장을 하는 함수
int COMMAND_CENTER::Connect_New_Client(int client_id) {
	//int i = Get_blank_index();
	int i = Get_index();
	if (i == -1) {
		printf("꽉차서 더 연결 불능\n");
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
	
	printf("[%d]님이 접속종료하셨습니다.\n", id);
}

// index 중 해당 id가 포함된 index 혹은 빈 인덱스를 찾는 함수
// -1 : 빈 인덱스 , 그 외 정수 : 해당 아이디
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

// 특정 클라이언트의 포트를 찾는 함수
int COMMAND_CENTER::Get_port(int id) {
	if (users[Get_index(id)].get_id() == -1)
		return -1;

	return users[Get_index(id)].get_port(1);
}

// 새로 연결된 클라이언트와 연결되어있는 클라이언트 서로 식별
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