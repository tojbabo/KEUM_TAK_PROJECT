#include "_CLASS.h"

using namespace std;

void CORE::Notify_Enter(SOCKET sock, ROOM r) {
	// ��Ʈ ��ȣ �ο�
	//_System_Msg_Send(sock, "Enter the room.");
	char msg[BUF_SIZE];

	//sprintf(msg, "#,%d,%s,%d\n",r.Get_ID(), r.Get_TITLE(), r.Get_PORT());

	r.SetMan(r.Get_PORT());
	strcpy(msg, r.Get_State().c_str());
	msg[0] = '#';

	send(sock, msg, strlen(msg), 0);
}

// �� ��ȣ ���� �� ��ȯ
int CORE::Create_RoomID() {
	
	if (RoomList.size() == 0)
		return 1;

	int num = 1;
	list<ROOM>::iterator ptr;

	for (ptr = RoomList.begin(); ptr != RoomList.end(); ptr++) {
		if (num != ptr->Get_ID())
			break;
		num++;
	}
	return num;
}

// �� ��� ���� �Լ�
void CORE::Response_List(SOCKET sock) {
	list<ROOM>::iterator ptr;
	string total = "";
	string temp;
	const char* buf;
	for (ptr = RoomList.begin(); ptr != RoomList.end(); ptr++) {
		temp = ptr->Get_State();
		total += temp;
	}
	buf = total.c_str();


	send(sock, buf, strlen(buf), 0);
}

// �� ���� �Լ�
ROOM CORE::Create_Room(char* str) {
	char str_temp[15];
	char _title[100];
	char _passwd[100];
	ROOM* temp=NULL;

	int room_num = Create_RoomID();
	
	strcpy(str_temp, str);

	char* ptr = strtok(str, ",");
	ptr = strtok(NULL, ",");
	//sscanf(ptr, "%s", _title);
	strcpy(_title, ptr);

	ptr = strtok(NULL, ",");
	if (ptr[0] == '@') {
		sprintf(_passwd, "");
	}
	else {
		sscanf(ptr, "%s", _passwd);
		ptr = strtok(NULL, ",");
	}
	string passwd(_passwd);
	string title(_title);

	temp = new ROOM(room_num, 1, title, passwd);
	if (temp != NULL) {
		ptr = strtok(NULL, ","); // ȭ 1
		temp->emo[0] = atoi(ptr);
		ptr = strtok(NULL, ","); // �� 2
		temp->emo[1] = atoi(ptr);
		ptr = strtok(NULL, ","); // �� 3
		temp->emo[2] = atoi(ptr);
		ptr = strtok(NULL, ","); // �� 4
		temp->emo[3] = atoi(ptr);
		ptr = strtok(NULL, ","); // �� 5
		temp->emo[4] = atoi(ptr);
		ptr = strtok(NULL, ","); // �� 6
		temp->emo[5] = atoi(ptr);
		ptr = strtok(NULL, ","); // �� 7
		temp->emo[6] = atoi(ptr);
	}
	

	// ���� ������ ���� �ڷ� �پ�� �ҋ�
	if (room_num > RoomList.back().Get_ID() || RoomList.size() == 0) {
		RoomList.push_back(*temp);
	}

	// ���� ������ ���� �߰��� �پ�� �Ҷ�
	else {
		list<ROOM>::iterator ptr;
		for (ptr = RoomList.begin(); ptr != RoomList.end(); ptr++) {
			if (ptr->Get_ID() > room_num) {
				temp = new ROOM(room_num, 1, title, passwd);
				RoomList.insert(ptr, *temp);
				break;
			}
		}
	}

	temp->Show();
	
	return *temp;
}

// �� ���� �Լ�
void CORE::IsEnterRoom(SOCKET sock, char* str) {
	int id;
	char passwd[10];
	char str_temp[15];
	strcpy(str_temp, str);

	char* c_ptr = strtok(str, ",");
	c_ptr = strtok(NULL, ",");
	sscanf(c_ptr, "%d", &id);

	if (str_temp[strlen(str_temp) - 2] == ',') {
		cout << "pass is null" << endl;
		sprintf(passwd, "");
	}
	else {
		c_ptr = strtok(NULL, ",");
		sscanf(c_ptr, "%s", &passwd);
	}

	ROOM target = *Search_Room(id);

	int temp = target.Enter(passwd);
	if (temp == CANNOT_ACCESS_LIMIT) {
		// ��� ����
		SysMsgSend_(sock, "POOLBANG.");
	}
	else if (temp == CANNOT_ACCESS_PASSWD) {
		// ��� Ʋ��
		SysMsgSend_(sock, "Not fit Password.");
	}
	else {

		Notify_Enter(sock, target);
	}
}


void CORE:: RequestPasswd(SOCKET sock, ROOM room) {
	char msg[50];
	sprintf(msg, "@,%d,%s\n", room.Get_ID(),room.Get_TITLE());
	send(sock, msg, strlen(msg), 0);
}

void CORE::Delete_Room(int id){
	cout << "deleted room id is : " << Search_Room(id)->Get_ID() << endl;;
	RoomList.erase(Search_Room(id));
}

list<ROOM>::iterator CORE::Search_Room(int ID) {
	list<ROOM>::iterator ptr;
	int i = 0;
	for (ptr = RoomList.begin(); ptr != RoomList.end(); ptr++, i++) {
		if (ptr->Get_ID() == ID)
			return ptr;
	}
	return ptr;
}
