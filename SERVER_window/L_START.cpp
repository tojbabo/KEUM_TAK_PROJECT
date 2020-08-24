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
	if (listen(sock, 30) == SOCKET_ERROR)
		Error_("SOCKET Listen Error in START.cpp");

	fd_set read, temp;
	TIMEVAL time;
	FD_ZERO(&read);
	FD_SET(sock, &read);

	/*
	char temp1[] = "@,첫번째_방,,@,1,2,3,4,5,6,7\n";
	char temp2[] = "@,욕설_금지방,,@,2,4,6,8,10,12,14\n";
	char temp3[] = "@,ㅋㅋㅋ크루킄ㅋㅋ,,@,3,6,9,12,15,18,21\n";
	char temp4[] = "@,외로운_사람만,,@,4,8,12,16,20,24,28\n";
	char temp5[] = "@,18男_메이플_여친괌,,@,5,10,15,20,25,30,35\n";
	char temp6[] = "@,니들은_이런거_배우지마라,,@,6,12,18,24,30,36,42\n";
	char temp7[] = "@,운세_봐드립니다.,,@,7,14,21,28,35,42,49\n";
	core.Create_Room(temp1);
	core.Create_Room(temp2);
	core.Create_Room(temp3);
	core.Create_Room(temp4);
	core.Create_Room(temp5);
	core.Create_Room(temp6);
	core.Create_Room(temp7);
	*/

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
		if (re == 0) 	continue;

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
						
						// !request - 리스트 요청
						if (Socket_Buffer[0] == '!') {													
							core.LOCK();
							core.Response_List(read.fd_array[i]);
							core.UNLOCK();
						}
						
						// @,TITLE,PASSWD - 방 생성
						else if (Socket_Buffer[0] == '@') {		
							cout << "방 생성임다 " << Socket_Buffer << endl;
							core.LOCK();
							ROOM room = core.Create_Room(Socket_Buffer);
							core.UNLOCK();

							char MessageToChild[100];
							sprintf(MessageToChild, "%d %d %d %d %d %d %d %d %s"
								, room.Get_PORT(),room.emo[0], room.emo[1], room.emo[2], 
								room.emo[3], room.emo[4], room.emo[5], room.emo[6],room.Get_TITLE());
							
							cout << "자식 프로세스 생성 ~!!~!"<<endl;
							ShellExecute(GetDesktopWindow(), _T("open"), _T("SERVER_window.exe"),MessageToChild, 0, SW_SHOWDEFAULT); 

							core.LOCK();
							core.Notify_Enter(read.fd_array[i], room);
							core.UNLOCK();



						}

						// #,RoomID,PASSWD  - 방 참여
						else if (Socket_Buffer[0] == '#') {												
							core.IsEnterRoom(read.fd_array[i],Socket_Buffer);
						}

						// $,RoomID - 방에서 사용자 나옴
						else if (Socket_Buffer[0] == '$') {
							int fromWhere;
							sscanf(Socket_Buffer, "$,%d", &fromWhere);
							int num = core.Search_Room(fromWhere)->Exit();
							cout << "numt is : " << num << endl;
							if (num == -1) {
								core.Delete_Room(fromWhere);
							}
							core.LOCK();
							core.Response_List(read.fd_array[i]);
							core.UNLOCK();
						}

						// ^,Roomid - 접속 하려는 방 선택
						else if (Socket_Buffer[0] == '^') {												
							int id;
							sscanf(Socket_Buffer, "^,%d", &id);
							ROOM r = *core.Search_Room(id);

							if (r.IsFree()) core.Notify_Enter(read.fd_array[i], r);
							else { core.RequestPasswd(read.fd_array[i], r); }
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

