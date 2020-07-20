#include "_LOGIC.h"
#include "_UTILITY.h"
#include "_CLASS.h"

using namespace cv;
using namespace std;

int LOGIC_watting(int PORT) {

#pragma region �⺻ ��¹� & ���� ���� �� �ʱ�ȭ

	cout << endl << "----------------------------------" << endl;
	cout << "[F_START.cpp] START<�θ�>" << endl;

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

	printf("���� ���� ���� ��ٸ��� �� \n");
	printf("IP : %s \nPORT : %d\n", IP, PORT);


#pragma endregion

	while (1) {
		temp = read;																			// �ӽ� �迭 ����
		time.tv_sec = 1;
		time.tv_usec = 0;

		re = select(NULL, &temp, NULL, NULL, &time);											// re�� select ����
		if (re == SOCKET_ERROR) break;
		if (re == 0) continue;

		for (int i = 0; i < read.fd_count; i++) {												// select ������ ��ŭ �ݺ�

#pragma region ����� �����


			if (FD_ISSET(read.fd_array[i], &temp))												// Ư�� ������ �ӽ� �迭�� �����ϴ��� Ȯ��
			{
				if (sock == read.fd_array[i]) {													// ������ ������ ���� ������ ���
					SOCKADDR_IN client_adr;
					int size = sizeof(client_adr);
					SOCKET client_sock = accept(sock, (SOCKADDR*)&client_adr, &size);			// ������ �߰� ����

					FD_SET(client_sock, &read);													// select �迭�� �ش� ���� ����
					printf("[start.cpp]New Connect user :			%d\n", client_sock);

					core.Response_List(client_sock);
					
				}
#pragma endregion
#pragma region  ����� ���� �۵���

				else {																			// ������ ������ ���� ������ �ƴ� �Ϲ� ������ ���

					int len = recv(read.fd_array[i], Socket_Buffer, sizeof(Socket_Buffer), 0);						// �б�
					if (len == 0 || len ==-1) {
						FD_CLR(read.fd_array[i], &read);
						printf("[%d]���� ���������ϼ̽��ϴ�.\n", read.fd_array[i]);
						closesocket(temp.fd_array[i]);
					}
					else {
						Socket_Buffer[len] = 0;
						printf("[start.cpp]recv message :				%s<%d>\n", Socket_Buffer, read.fd_array[i]);

						if (Socket_Buffer[0] == '!') {													// !request - ����Ʈ ��û
							core.Response_List(read.fd_array[i]);
						}

						else if (Socket_Buffer[0] == '@') {												// @,TITLE,PASSWD - �� ����
							ROOM room = core.Create_Room(Socket_Buffer);

							char MessageToChild[100];
							sprintf(MessageToChild, "%d %d %d %d %d %d %d %d"
								, room.Get_PORT(),room.emo[0], room.emo[1], room.emo[2], 
								room.emo[3], room.emo[4], room.emo[5], room.emo[6]);
							
							cout << "�ڽ� ���μ��� ���� ~!!~!"<<endl;
							ShellExecute(GetDesktopWindow(), _T("open"), _T("temp.exe"),MessageToChild, 0, SW_SHOWDEFAULT); 

							core.Notify_Enter(read.fd_array[i], room.Get_TITLE(), room.Get_PORT());
						}

						else if (Socket_Buffer[0] = '#') {												// #,ID,PASSWD  - �� ����
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

