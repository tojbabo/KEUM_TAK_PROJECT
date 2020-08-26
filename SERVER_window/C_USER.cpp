#include "_CLASS.h"

USER::USER(int id, int port) {
	head = THREAD_();
	id = id;
	port = port;
	Runner = false;
}
void USER::release() {
	Runner = false;
	head.t.join();

	head = THREAD_();

	id = -1;
	port = port - port%10;
}
std::thread* USER::get_thread(int other) {
	THREAD_* ptr = &head;

	if (ptr == NULL) {
		head = THREAD_();
		return head.get_thread(id, other);
	}

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
	node = new THREAD_();
	p->link = node;
}
void USER::release_thread() {
	THREAD_* ptr = &head;
	THREAD_* pre = NULL;
	while (1) {
		if (ptr->link == NULL)
			break;
		pre = ptr;
		ptr = ptr->link;
		
	}
}
void USER::set_name(char* name_) {
	strcpy(name, name_);
}
void USER:: Send_Msg(char* msg) {
	int num = send(id, msg, strlen(msg),0);
	/*if (num == 0 || num == -1) {
		cout << "소켓이 끊어졌습니다.["<<num<<"]";
		cout << strerror(errno) << endl;
	}*/
}