#include <windows.h>
#include <iostream>
#include "filesystem.h"
#include "tools.h"
using namespace std;

int main()
{
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
		wcout << it->name << L" - " << it->is_directory << L" - " << it->date_time << endl;

	wcout << L"Hällo Welt" << endl;
}