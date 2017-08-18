#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include "web_server.h"
using namespace std;

void Web_server::start()
{
	listener_thread = make_unique<thread>([this]() -> void { this->run_listener(); });
	listener_thread->detach();
}

void Web_server::stop()
{
	closesocket(listen_socket);
	WSACleanup();
}

void Web_server::run_listener()
{
	WSADATA wsaData{ 0 };
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR)
	{
		auto err = WSAGetLastError();
		return;
	}

	listen_socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN internet_addr;
	internet_addr.sin_family = AF_INET;
	internet_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	internet_addr.sin_port = htons(20002);
	::bind(listen_socket, reinterpret_cast<PSOCKADDR>(&internet_addr), sizeof(internet_addr));
	listen(listen_socket, SOMAXCONN);

	SOCKADDR_IN sockaddr_remote{ 0 };
	int remote_len = sizeof(sockaddr_remote);
	while (true)
	{
		auto accept_socket = WSAAccept(listen_socket, reinterpret_cast<SOCKADDR*>(&sockaddr_remote), &remote_len, nullptr, reinterpret_cast<DWORD_PTR>(nullptr));
		if (accept_socket == reinterpret_cast<SOCKET>(INVALID_HANDLE_VALUE))
		{
			auto err = WSAGetLastError();
			if (err == WSAEINTR)
				break;
		}
	}
}


