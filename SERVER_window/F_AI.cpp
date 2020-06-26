#include"temp.h"
#include "_UTILITY.h"




//testing
TCHAR szName[] = TEXT("temp");
TCHAR szMsg[] = TEXT("이거 보이면 성공한거");

int func1() {


	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}
	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(hMapFile);

		return 1;
	}
	TCHAR buf[100];
	CopyMemory((PVOID)pBuf, "start\0", (_tcslen("start\0") * sizeof(TCHAR)));



	while (1) {

		if (pBuf[0] == '1') {
			//printf("it's not my turn wait......\n");
			Sleep(500);
			continue;
		}


		printf("input > ");
		scanf("%s", buf);
		sprintf(buf, "1%s\0", buf);
		CopyMemory((PVOID)pBuf, buf, (_tcslen(buf) * sizeof(TCHAR)));


		if (!strcmp(buf, "no"))
		{
			getch();
			break;
		}
	}

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}
void func2() {
	printf("goooooooooood\n");
}
int func3() {
	printf("new version\n");
	HANDLE hMapFile;
	LPCTSTR pBuf;
	int num = 0;

	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		szName);               // name of mapping object

	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not open file mapping object (%d).\n"),
			GetLastError());
		return 1;
	}

	pBuf = (LPTSTR)MapViewOfFile(hMapFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		BUF_SIZE);



	while (1) {
		if (pBuf == NULL)
		{
			_tprintf(TEXT("Could not map view of file (%d).\n"),
				GetLastError());

			CloseHandle(hMapFile);

			return 1;
		}

		if (pBuf[0] == '0') {
			//printf("it's write time.. not my turn .. \n");
			Sleep(1000);
			continue;
		}

		printf("%s\n", pBuf);
		CopyMemory((PVOID)pBuf, "0\0", (_tcslen("0\0") * sizeof(TCHAR)));

		if (!strcmp(pBuf, "no"))
			break;
		//num++;
	}

	//MessageBox(NULL, pBuf, TEXT("Process2"), MB_OK);

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);

	return 0;
}


void Create_AiProcess() {
	char title[] = "SERVER_window.exe"; // ai 프로그램 이름 입력
	char key[] = "key";					// 공유 메모리 키
	ShellExecute(GetDesktopWindow(), _T("open"), _T(title), key/*shared memery key*/, 0, SW_SHOWDEFAULT);
	printf("Process open : %s <%s>\n", title,key);

	MemoryShare_AiProcess(key);
}
int MemoryShare_AiProcess(LPCSTR key) {

	HANDLE hMapFile;
	LPCTSTR pBuf;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		BUF_SIZE,                // maximum object size (low-order DWORD)
		key);					 // name of mapping object

	if (hMapFile == NULL)
		Error_("hMapFile Error in F_AI.cpp");

	pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		BUF_SIZE);

	if (pBuf == NULL)
	{
		CloseHandle(hMapFile);
		Error_("pBuf Error in F_AI.cpp");
	}

	////////////////////////
	// shared memory에 데이터 입력
	CopyMemory((PVOID)pBuf, "start\0", (_tcslen("start\0") * sizeof(TCHAR)));

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);
	////////////////////////
	return 0;
}