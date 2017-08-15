#include <iostream>
#include "filesystem.h"
using namespace std;

int main()
{
	auto drives = get_drives();
	for (auto it = drives.begin(); it < drives.end(); it++)
		wcout << it->name << L" - " << static_cast<int>(it->type) << L" - " << it->description << L" - " << it->size << endl;

	auto files = list_directory(L"c:\\windows\\system32");
	for (auto it = files.cbegin(); it < files.cend(); it++)
		wcout << it->name << L" - " << it->is_directory << endl;

	wcout << L"Hällo Welt" << endl;
}