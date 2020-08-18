#include "_SHARED.h"
#include <tchar.h>


#define size1 230400+10
#define size2 921600+10

using namespace std;
using namespace cv;

HANDLE File_Mapping(char* key) {
	HANDLE hMapFile;

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size1,	                 // maximum object size (low-order DWORD)
		key);					 // name of mapping object

	//cout << "mapping file is : " << key << endl;
	if (hMapFile == NULL)
	{
		cout << "Could not create file mapping ojbect" << endl;
		return NULL;
	}
	return hMapFile;
}

LPCTSTR Make_Shared_Memory(HANDLE h) {
	LPCTSTR pBuf;

	pBuf = (LPTSTR)MapViewOfFile(h,          // handle to map object
		FILE_MAP_ALL_ACCESS,				 // read/write permission
		0,
		0,
		size1);

	if (pBuf == NULL)
	{
		cout << "Could not map view of file" << endl;
		CloseHandle(h);
		return NULL;
	}
	return pBuf;
}

void Shared_Clear(LPCTSTR* m, HANDLE* h ,int num) {
	for (int i = 0; i < num; i++) {
		UnmapViewOfFile(m[i]);
		CloseHandle(h[i]);
	}
}


LPCSTR CreateMemory(HANDLE* h, char* szName) {
	//TCHAR szName[] = TEXT(memory);
	LPCSTR SM;

	*h = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		NULL,                    // default security
		PAGE_READWRITE,          // read/write access
		0,                       // maximum object size (high-order DWORD)
		size1,	                 // maximum object size (low-order DWORD)
		szName);                 // name of mapping object

	if (*h == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n"),
			GetLastError());
		return NULL;
	}

	SM = (LPTSTR)MapViewOfFile(*h,   // handle to map object
		FILE_MAP_ALL_ACCESS,				 // read/write permission
		0,
		0,
		size1);

	if (SM == NULL)
	{
		_tprintf(TEXT("Could not map view of file (%d).\n"),
			GetLastError());

		CloseHandle(*h);

		return NULL;
	}

	return SM;
}