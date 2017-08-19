#pragma once
#include <winsock2.h>
#include <array>
using namespace std;

class Session : public OVERLAPPED
{
public:
	Session(HANDLE socket);
	void read();
	void on_read(int bytes_read);
private:
	HANDLE socket;
	array<char, 1000> buffer;
	string header;
};

