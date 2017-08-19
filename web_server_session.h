#pragma once
#include <winsock2.h>
#include <array>
#include <vector>
using namespace std;

// TODO: Wenn der Server heruntergefahren wird, werden die Sessions nicht beendet und gelöscht

class Session : public OVERLAPPED
{
public:
	Session(HANDLE socket);
	~Session();
	void read();
private:
	void on_read(int bytes_read);
	void on_written(int bytes_read);
	void write();
	void send_not_found();

	HANDLE socket;
	array<char, 1000> buffer;
	string header;
	
	vector<char> http_response;
	int written{ 0 };
};

