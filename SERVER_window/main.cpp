#include "_LOGIC.h"
#include<tchar.h>

using namespace std;

int main(int num, char* args[]) {

	cout << VERSION << endl;
	cout << COMMENT << endl;
	int port = 9000;
	int opt[7] = { 0,0,0,0,0,0,0 };

	system("title 부모 프로세스");

	if (num <= 1) cout << "default\n";
	else { 
		char title[50];
		sscanf(args[1], "%d", &port);
		sscanf(args[2], "%d", &opt[0]);
		sscanf(args[3], "%d", &opt[1]);
		sscanf(args[4], "%d", &opt[2]);
		sscanf(args[5], "%d", &opt[3]);
		sscanf(args[6], "%d", &opt[4]);
		sscanf(args[7], "%d", &opt[5]);
		sscanf(args[8], "%d", &opt[6]);
		sprintf(title, "title %s", args[9]);
		system(title);
	}
	
	cout << "ip : " << IP << endl;
	cout << "port : " << port << endl;

	//부모
	//return LOGIC_watting(port);
	//자식
	//
	return LOGIC_chatting(port,opt);

}