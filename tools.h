#pragma once
#include <string>
#include <vector>

extern const std::vector<std::wstring> split(const std::wstring &s, wchar_t delim);

class file_handle
{
public:
	file_handle(HANDLE handle) : handle(handle) {}
	~file_handle();
	operator HANDLE() const { return handle; }
private:
	HANDLE handle{ INVALID_HANDLE_VALUE };
};
