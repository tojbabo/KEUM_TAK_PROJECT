/*#ifdef _DEBUG
	#undef _DEBUG
	#include<Python.h>
	#define _DEBUG
#else	
	#include<python.h>
#endif

#include<stdio.h>

class t {
public:
	int num;
	t* link;
	t() { link = NULL; num = 0; }
};
class U {
public:
	t* head;
	U() {
		printf("obj start\n");
		con(head);
	}
	void input(int num) {
		t* p = head->link;
		while (1)
		{
			if (p->num == 0) {
				p->num = num;
				break;
			}
			else {
				printf("%d - ", p->num);
				con(p);
				p = p->link;
			}
		}
	}
	void con(t* p) {
		t* a;
		a = new t;
		p->link = a;
	}
};

int main() {
	printf("start\n");
	U a;
	a.input(3);
	a.input(4);
	a.input(5);
	// 객체 생성
	// 데이터 입력
	printf("end\n");
}

/*
int main() {
	
	PyObject* pName, * pModule, * pFunc, * pValue;
	printf("check 1\n");
	Py_Initialize();
	pName = PyUnicode_FromString("test2");					// 대상 파일 불러오기
	pModule = PyImport_Import(pName);						// 불러온 파일 임포트
	if (pModule != NULL) {
		printf("not null\n");

		pFunc = PyObject_GetAttrString(pModule, "say");		// 함수 불러오기
		PyObject_CallObject(pFunc, NULL);					// 함수 사용  ----- 인자 전달 및 결과 값 받는법 조사 必
	}
	else
		printf("is null\n");
	//PyRun_SimpleString("import test2.py\n");
	//PyRun_SimpleString("sys.stdout.write('Hello from an embedded Python Script\\n')\n");
	Py_Finalize();

	printf("check 2\n");
	return 0;
}
*/
/**/


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