#include"_CLASS.h"



ROOM::ROOM() {
	room_id = person_num = 0;
	passwd = "";
	title = "";
}

	// 방 생성
ROOM::ROOM(int id, int num, string name, string pass) {
	cout<<"passwd is : "<<pass<<endl;
	room_port = 9001 + (id-1) * 100;
	room_id = id;
	person_num = num;
	passwd = pass;
	title = name;
}

int ROOM::Get_ID() { return room_id; }

int ROOM::Get_PORT() { return room_port; }

string ROOM::Get_TITLE() { return title; }

string ROOM::Get_State() {
	string temp ="!,"+ to_string(room_id) + "," + title + "," + to_string(person_num) + "\n";
	return temp;
}

	// 방에 접근
int ROOM::Enter(char* pass) {
	cout << passwd <<"/"<<pass<< endl;
	cout << pass << "/" << passwd << endl;

	if (passwd != pass)
		return 1;

	if (person_num == limit)
		return 2;									// 사람 꽉 참

	person_num++;
	return 0;										// 접근 ㅇㅋ
}

void ROOM::Show() {
	cout << "============================" << endl;
	cout << "room number : " << room_id << endl;
	cout << "room title  : " << title << endl;
	cout << "room passwd : " << passwd << endl;
	cout << "room port   : " << room_port << endl;
	cout << "============================" << endl;
}