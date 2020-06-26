#include "_UTILITY.h"
#include <string>
using namespace cv;
using namespace std;

// 주어진 포트와 옵션으로 소켓 생성
SOCKET Initialize(int port, int opt) {
	// opt -- 1: TCP , 2 : UDP
	SOCKADDR_IN bind_adr;
	Memset_(&bind_adr, IP, port);

	SOCKET sock;
	if (opt == 1)
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else if (opt == 2)
		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	else {
		cout << "wrong initalize option (only 1 or 2) \n";
		exit(-1);
	}

	if (::bind(sock, (struct sockaddr*) & bind_adr, sizeof(bind_adr)) == SOCKET_ERROR) {
		cout << "binding error \n";
		exit(-1);
	}

	return sock;
}

// 입력된 값에 따라서 주소함수 초기화 함수
void Memset_(SOCKADDR_IN* adr, const char* ip, int port) {
	memset(adr, 0, sizeof(*adr));
	adr->sin_family = AF_INET;
	adr->sin_addr.s_addr = inet_addr(ip);
	adr->sin_port = htons(port);
}

void Error_(const char* str) {
	cout << "*************[ERROR]"<<str<<endl;
	exit(-1);
}
