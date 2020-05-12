
#include <windows.h>
#include <cstring>
#include <cstdbool>
#include <iostream>
#include <Windows.h>
#include <tchar.h>
using namespace std;

//트리거 완성
//트리거 + 사진 넣으면 됨

typedef struct
{
	void* hFileMap;
	void* pData;
	char MapName[256];
	size_t Size;
} SharedMemory;

bool CreateMemoryMap(SharedMemory* shm)
{
	if ((shm->hFileMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, shm->Size, shm->MapName)) == NULL)
	{
		return false;
	}

	if ((shm->pData = MapViewOfFile(shm->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, shm->Size)) == NULL)
	{
		CloseHandle(shm->hFileMap);
		return false;
	}
	return true;
}

bool FreeMemoryMap(SharedMemory* shm)
{
	if (shm && shm->hFileMap)
	{
		if (shm->pData)
		{
			UnmapViewOfFile(shm->pData);
		}

		if (shm->hFileMap)
		{
			CloseHandle(shm->hFileMap);
		}
		return true;
	}
	return false;
}

void make_python_process(SharedMemory *shm) {
	char option[] = "open";
	// char title[] = "read_write.exe";
	char title[] = "count_to_10.exe";
	char parameter[] = "";
	int count = 0;

	// ShellExecute(핸들값, 수행동작 지정, 프로그램 파일명, 수행시 파라미터, 파일 경로, 수행시 윈도우 크기 등)
	if (ShellExecute(GetDesktopWindow(), _T(option), _T(title), _T(parameter), 0, SW_SHOWDEFAULT))
		printf("Success !!!");
	else
		printf("nonononono");
	
	while (1) {
		shm->pData = (LPTSTR) MapViewOfFile(shm->hFileMap, FILE_MAP_ALL_ACCESS, 0, 0, shm->Size);
		char* ptr = (char*)(*shm).pData;
		memset(ptr, 0, (*shm).Size);
		int count = 0;
			
		// shared memory에 적힐때까지 대기
		// error trigger 만들어야함
		while (ptr && (*ptr == 0))
		{
			Sleep(100);
		}
		
		int size = (int)*ptr;
		ptr += sizeof(char);

		/*
		int i = 0;
		for (; i < size; ++i)
		{
			 std::cout << ptr[i];
			if(i == size-1)
				cout << endl;
		}
		*/
		cout << "ptr = " << ptr << endl;

		// 트리거 역할 - 10 일때 return 적고 아니면 next 적음
		if (strcmp(ptr, "10") == 0) {
			TCHAR message[] = TEXT("return");
			strcpy((char*)shm->pData, message);
			CopyMemory((PVOID)shm->pData, message, (_tcslen(message) * sizeof(TCHAR)));
		}
		else {
			cout << "write next!" << endl;
			TCHAR message[] = TEXT("next");
			strcpy((char*)shm->pData, message);
			CopyMemory((PVOID)shm->pData, message, (_tcslen(message) * sizeof(TCHAR)));
		}
		Sleep(1000);
	}
}

int main()
{
	SharedMemory shm = { 0 };
	shm.Size = 512;
	sprintf(shm.MapName, "Local\\Test");

	if (CreateMemoryMap(&shm))
	{
		make_python_process(&shm);
		FreeMemoryMap(&shm);
	}
}
