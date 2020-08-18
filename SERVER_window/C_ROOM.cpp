#include "_CLASS.h"



ROOM::ROOM() {
	room_id = person_num = 0;
	passwd = "";
	title = "";
}

	// 방 생성
ROOM::ROOM(int id, int num, string name, string pass) {
	//cout<<"passwd is : "<<pass<<endl;
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
	string temp ="!,"+ to_string(room_id) + "," + title + "," + to_string(person_num)  
		+ "," + to_string(emo[0]) + "," + to_string(emo[1]) + "," + to_string(emo[2]) 
		+ "," + to_string(emo[3]) + "," + to_string(emo[4]) + "," + to_string(emo[5])
		+ "," + to_string(emo[6]) +"\n";

	return temp;
}

	// 방에 접근
int ROOM::Enter(char* pass) {
	cout << passwd <<"/"<<pass<< endl;
	cout << pass << "/" << passwd << endl;

	if (passwd != pass)
		return CANNOT_ACCESS_PASSWD;

	if (person_num == limit)
		return CANNOT_ACCESS_PASSWD;				// 사람 꽉 참

	person_num++;
	return room_port;										// 접근 ㅇㅋ
}

int ROOM::Exit() {
	person_num--;
	cout << "person num is :  " << person_num << endl;
	if (person_num == 0)
		return -1;
	return 0;
}

void ROOM::Show() {
	cout << "============================" << endl;
	cout << "room number : " << room_id << endl;
	cout << "room title  : " << title << endl;
	cout << "room passwd : " << passwd << endl;
	cout << "room port   : " << room_port << endl;
	printf("room emo    : [%d/%d/%d/%d/%d/%d/%d]\n",
		emo[0], emo[1], emo[2], emo[3], emo[4], emo[5], emo[6]);
	cout << "============================" << endl;
}