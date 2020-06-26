#include "_CLASS.h"

USER::USER() {
	head = new THREAD_;
	id = 0;
	port = 0;
}
std::thread* USER::get_thread(int idx = -1) {
	THREAD_* ptr = head;
	int num = 0;
	while (1) {
		if (ptr->link == NULL && idx == -1) {
			make_thread(ptr);
			return ptr->get_thread();
		}
		else if (idx == num)
			return ptr->get_thread();
		else {
			ptr = ptr->link;
			num++;
		}
	}
}
void USER::make_thread(THREAD_* p) {
	THREAD_* node;
	node = new THREAD_;
	p->link = node;
}
void USER::release_thread() {
	THREAD_* ptr = head;
	while (1) {
		if (ptr == NULL)
			break;
	}
}
void USER::input_data(int id_, int port_) {
	id = id_;
	port = port_;
}
void USER::set_name(char* name_) {
	strcpy(name, name_);
}
void USER:: Send_Msg(char* msg) {
	send(id, msg, strlen(msg),0);
}