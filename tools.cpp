#include <windows.h>
#include <sstream>
#include <iostream>
#include <string>
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

wstring convert_to_utf16(string utf8)
{
	wstring utf16;
	auto utf8length = static_cast<int>(utf8.length());
	auto utf16Length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), utf8length, nullptr, 0);
	utf16.resize(utf16Length);
	int result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.data(), utf8length, &utf16[0], utf16Length);
	return move(utf16);

}