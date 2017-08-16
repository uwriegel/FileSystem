#pragma once
#include <string>
#include <vector>
using namespace std;

extern const vector<wstring> split(const wstring &s, wchar_t delim);
extern wstring convertToUtf16(string utf8);

class file_handle
{
public:
	file_handle(HANDLE handle) : handle(handle) {}
	~file_handle();
	operator HANDLE() const { return handle; }
private:
	HANDLE handle{ INVALID_HANDLE_VALUE };
};
