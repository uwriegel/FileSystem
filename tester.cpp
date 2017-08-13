#include <iostream>
#include "filesystem.h"
using namespace std;

int main()
{
	auto drives = get_drives();
	for (auto it = drives.cbegin(); it < drives.cend(); it++)
		wcout << it->name << L" - " << static_cast<int>(it->type) << L" - " << it->description << endl;

	wcout << L"Hällo Welt" << endl;
}