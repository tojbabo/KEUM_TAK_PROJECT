#include "_CLASS.h"

USER::USER(int id, int port) {
	head = new THREAD_;
	id = id;
	port = port;
}
void USER::release() {
	head = new THREAD_;
	id = -1;
	port = port - port%10;
}
std::thread* USER::get_thread(int other) {
	THREAD_* ptr = head;
	while (1) {
		if (ptr->link == NULL ) {
			make_thread(ptr);
			return ptr->get_thread(id,other);
		}
		else {
			ptr = ptr->link;
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