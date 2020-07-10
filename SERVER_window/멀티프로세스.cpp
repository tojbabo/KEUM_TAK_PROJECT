#include"temp.h"

int main(int num, char* str[]) {

	/*
		char title[] = "SERVER_window.exe";
	ShellExecute(GetDesktopWindow(), _T("open"), _T(title), "ee", 0, SW_SHOWDEFAULT);
	printf("open process %s\n", title);


	func2();
	func1();
	func2();

	system("pause");
	/*
	

	
	int i = 0;

	if (num == 0)
		printf("전달된 인자 없음\n");

	else {

		printf("전달된 인자 수 : %d\n", num);

		for (i = 0; i < num; i++) {
			printf("%d : %s\n", i, str[i]);
		}
	}

	func3();

	system("pause");
	*/
	TCHAR szName[] = TEXT("temp");
	TCHAR szMsg[] = TEXT("이거 보이면 성공한거");

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
	CopyMemory((PVOID)pBuf, "stardsfdfsdf dsfdt\0", (_tcslen("stardsfdfsdf dsfdt\0") * sizeof(TCHAR)));

	printf("result : %s\n", pBuf);
	CopyMemory((PVOID)pBuf, "1\0", (_tcslen("1\0") * sizeof(TCHAR)));
	
	

	printf("result : %s\n", pBuf);

	UnmapViewOfFile(pBuf);

	CloseHandle(hMapFile);
	printf("input > ");
	fgets(buf, 100,stdin);
	printf("%s", buf);


	return 99;
}