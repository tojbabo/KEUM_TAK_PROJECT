#include "_LOGIC.h"
#include<tchar.h>

using namespace std;
/**/
int main(int num, char *args[]) {

	cout << VERSION << endl;
	cout << COMMENT << endl;
	int port = 9000;
	if (num <= 1) {
		cout << "default\n";
		cout << "ip : " << IP << endl;
		cout << "port : " << port << endl;
	}
	else {
		sscanf(args[1], "%d", &port);
		cout << "ip : " << IP << endl;
		cout << "port : " << port << endl;
	}

	//부모
	//LOGIC_watting(port);

	//자식
	LOGIC_chatting(port);

	return 0;
}
