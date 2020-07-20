#include "_LOGIC.h"
#include "_UTILITY.h"
#include "_CLASS.h"

using namespace cv;
using namespace std;

int LOGIC_watting(int PORT) {

#pragma region 기본 출력문 & 변수 선언 및 초기화

	cout << endl << "----------------------------------" << endl;
	cout << "[F_START.cpp] START<부모>" << endl;

	CORE core;

	WSADATA wsaData;
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
		Error_("WSAStartup Error in START.cpp");

	SOCKET sock = Initialize_(PORT, 1);
	if (listen(sock, 5) == SOCKET_ERROR)
		Error_("SOCKET Listen Error in START.cpp");

	fd_set read, temp;
	TIMEVAL time;
	FD_ZERO(&read);
	FD_SET(sock, &read);

	int re;
	char Socket_Buffer[BUF_SIZE];

	printf("대기방 서버 열고 기다리는 중 \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);


#pragma endregion

	while (1) {
		temp = read;																			// 임시 배열 설정
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re에 select 설정
		if (re == SOCKET_ERROR) break;
		if (re == 0) continue;

		for (int i = 0; i < read.fd_count; i++) {												// select 사이즈 만큼 반복

#pragma region 사용자 연결부


			if (FD_ISSET(read.fd_array[i], &temp))												// 특정 소켓이 임시 배열에 존재하는지 확인
			{
				if (sock == read.fd_array[i]) {													// 반응된 소켓이 메인 소켓일 경우
					SOCKADDR_IN client_adr;
					int size = sizeof(client_adr);
					SOCKET client_sock = accept(sock, (SOCKADDR*)&client_adr, &size);			// 소켓을 추가 생성

					FD_SET(client_sock, &read);													// select 배열에 해당 소켓 설정
					printf("[start.cpp]New Connect user :			%d\n", client_sock);

					core.Response_List(client_sock);
					
				}
#pragma endregion
#pragma region  사용자 정상 작동부

				else {																			// 반응된 소켓이 메인 소켓이 아닌 일반 소켓일 경우

					int len = recv(read.fd_array[i], Socket_Buffer, sizeof(Socket_Buffer), 0);						// 읽기
					if (len == 0 || len ==-1) {
						FD_CLR(read.fd_array[i], &read);
						printf("[%d]님이 접속종료하셨습니다.\n", read.fd_array[i]);
						closesocket(temp.fd_array[i]);
					}
					else {
						Socket_Buffer[len] = 0;
						printf("[start.cpp]recv message :				%s<%d>\n", Socket_Buffer, read.fd_array[i]);

						if (Socket_Buffer[0] == '!') {													// !request - 리스트 요청
							core.Response_List(read.fd_array[i]);
						}

						else if (Socket_Buffer[0] == '@') {												// @,TITLE,PASSWD - 방 생성
							ROOM room = core.Create_Room(Socket_Buffer);

							char MessageToChild[100];
							sprintf(MessageToChild, "%d %d %d %d %d %d %d %d"
								, room.Get_PORT(),room.emo[0], room.emo[1], room.emo[2], 
								room.emo[3], room.emo[4], room.emo[5], room.emo[6]);
							
							cout << "자식 프로세스 생성 ~!!~!"<<endl;
							ShellExecute(GetDesktopWindow(), _T("open"), _T("temp.exe"),MessageToChild, 0, SW_SHOWDEFAULT); 

							core.Notify_Enter(read.fd_array[i], room.Get_TITLE(), room.Get_PORT());
						}

						else if (Socket_Buffer[0] = '#') {												// #,ID,PASSWD  - 방 참여
							core.IsEnterRoom(read.fd_array[i],Socket_Buffer);
							
						}

						else {/* garvage data*/ }
					}
				}
			}
#pragma endregion
		}
	}
	return 0;
}

