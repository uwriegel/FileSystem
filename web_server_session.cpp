#include <regex>
#include "web_server_session.h"

Session::Session(HANDLE socket)
	: OVERLAPPED{ 0 }
	, socket(socket) 
{}

Session::~Session()
{
	closesocket(reinterpret_cast<SOCKET>(socket));
}

void Session::read()
{
	if (!ReadFileEx(socket, buffer.data(), static_cast<DWORD>(buffer.size()), this,
		[](DWORD error_code, DWORD read, OVERLAPPED *session) {
		reinterpret_cast<Session*>(session)->on_read(read);
	}))
		delete(this);
}

void Session::on_read(int bytes_read)
{
	if (!bytes_read)
	{
		delete this;
		return;
	}

	header += string(buffer.data(), bytes_read);
	if (header.find("\r\n\r\n") != string::npos)
	{
		string firstline = header.substr(0, header.find("\r\n"));
		regex extract_file("/Icon\\?File=(.+)\\sHTTP");
		smatch matches;
		if (regex_search(firstline, matches, extract_file))
		{
			auto file = matches[1].str();
		}
		else
			send_not_found();
	}

	read();
}

void Session::on_written(int bytes_written)
{
	if (!bytes_written)
		return;

	written += bytes_written;
	if (http_response.size() <= written)
		return;

	write();
}

void Session::write()
{
	if (!WriteFileEx(socket, http_response.data() + written, http_response.size() - written, this,
		[](DWORD error_code, DWORD written, OVERLAPPED *session) {
		reinterpret_cast<Session*>(session)->on_written(written);
	}))
		delete this;
}

void Session::send_not_found()
{
	auto not_found = "Not found"s;
	auto response =
R"(HTTP/1.1 404 Not Found
Content-Length: )"s + to_string(not_found.length()) + R"(
Content-Type: text/html; charset=UTF-8

)"s + not_found;

	copy(response.data(), response.data() + response.length(), back_inserter(http_response));

	write();
}