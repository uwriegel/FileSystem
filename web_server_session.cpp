#include <regex>
#include "web_server_session.h"
#include "icon_extractor.h"

Session::Session(HANDLE socket)
	: OVERLAPPED{ 0 }
	, socket(socket) 
{
	gdiplus_initialize();
}

Session::~Session()
{
	closesocket(reinterpret_cast<SOCKET>(socket));
	gdiplus_uninitialize();
}

void Session::read()
{
	header.clear();
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
		regex extract_file("/Icon\\?file=(.+)\\sHTTP");
		smatch matches;
		if (regex_search(firstline, matches, extract_file))
		{
			auto file = matches[1].str();
			auto bytes = extract_icon(file);
			send_image(bytes);
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
	if (!WriteFileEx(socket, http_response.data() + written, static_cast<DWORD>(http_response.size() - written), this,
		[](DWORD error_code, DWORD written, OVERLAPPED *session) {
		reinterpret_cast<Session*>(session)->on_written(written);
	}))
		delete this;
}

void Session::send_image(const vector<char>& bytes)
{
	http_response.resize(0);
	written = 0;
	auto response =
		R"(HTTP/1.1 200 OK
Content-Type: image/png
Content-Length: )"s + to_string(bytes.size()) + R"(
Last-Modified: Sat, 12 Aug 2017 18:53:48 GMT

)"s;
	copy(response.data(), response.data() + response.length(), back_inserter(http_response));
	copy(bytes.data(), bytes.data() + bytes.size(), back_inserter(http_response));

	write();
}

void Session::send_not_found()
{
	http_response.resize(0);
	written = 0;
	auto not_found = "Not found"s;
	auto response =
R"(HTTP/1.1 404 Not Found
Content-Length: )"s + to_string(not_found.length()) + R"(
Content-Type: text/html; charset=UTF-8

)"s + not_found;

	copy(response.data(), response.data() + response.length(), back_inserter(http_response));

	write();
}