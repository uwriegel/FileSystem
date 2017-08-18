#pragma once
#include <thread>
using namespace std;

class Web_server
{
public:
	void start();
	void stop();
private:
	void run_listener();

	unique_ptr<thread> listener_thread;
	SOCKET listen_socket{ 0 };
};
