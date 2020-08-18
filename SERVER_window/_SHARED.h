//#ifndef SHARED
//#define SHARED

#include "_HEADER.h"

HANDLE File_Mapping(char* Mapping_file_name);
LPCTSTR Make_Shared_Memory(HANDLE Handler);
void Shared_Clear(LPCTSTR* Mapping_file_name, HANDLE* handler,int array_size);
LPCSTR CreateMemory(HANDLE* HANDLE_, char* Memroy_name);

//#endif