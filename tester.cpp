#include <windows.h>
#include <iostream>
#include <fstream>
#include "filesystem.h"
#include "tools.h"
#include "web_server.h"
#include "icon_extractor.h"
using namespace std;

int main()
{
	gdiplus_initialize();
	auto bytes = extract_icon(".docx"); 
	ofstream outfile(LR"(c:\users\urieg\desktop\schwein.png)", ofstream::binary);
	outfile.write(bytes.data(), bytes.size());
	outfile.flush();
	outfile.close();
	gdiplus_uninitialize();

	Web_server web_server;
	web_server.start();
	wcout << L"Web server running" << endl;
	string line; 
	getline(cin, line);
	web_server.stop();

	SYSTEMTIME st;
	GetSystemTime(&st);
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	wcout << format_time(ft) << endl;
	
	auto drives = get_drives();
	for (auto it = drives.begin(); it < drives.end(); it++)
		wcout << it->name << L" - " << static_cast<int>(it->type) << L" - " << it->description << L" - " << it->size << endl;

	//auto files = list_files(L"c:\\windows\\system32");
	auto files = list_files(L"c:\\");
	for (auto it = files.cbegin(); it < files.cend(); it++)
		wcout << it->name << L" - " << it->is_directory << L" - " << it->date_time << L" - " << it->size << endl;

	
	getline(cin, line);
	wcout << L"Hällo Welt" << endl;
}