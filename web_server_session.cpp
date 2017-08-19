#include "web_server_session.h"

Session::Session(HANDLE socket)
	: OVERLAPPED{ 0 }
	, socket(socket) 
{}

void Session::read()
{
	if (!ReadFileEx(socket, buffer.data(), static_cast<DWORD>(buffer.size()), this,
		[](DWORD error_code, DWORD read, OVERLAPPED *session) {
		reinterpret_cast<Session*>(session)->on_read(read);
	}))
	{
		auto aue = GetLastError();
		int u = 0;
	}
}
void Session::on_read(int bytes_read)
{
	if (!bytes_read)
	{
		closesocket(reinterpret_cast<SOCKET>(socket));
		delete this;
		return;
	}

	header += string(buffer.data(), bytes_read);
	if (header.find("\r\n\r\n") != string::npos)
	{
		int u = 0;
	}

	read();
}

