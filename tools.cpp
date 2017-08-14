#include <windows.h>
#include <sstream>
#include "tools.h"
using namespace std;

const vector<wstring> split(const wstring &s, wchar_t delim)
{
	wstringstream wss(s);
	wstring item;
	vector<wstring> elems;
	while (getline(wss, item, delim))
		elems.push_back(move(item));

	return elems;
}

file_handle::~file_handle() 
{ 
	if (handle != INVALID_HANDLE_VALUE)
		CloseHandle(handle); 
}