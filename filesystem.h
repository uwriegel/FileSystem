#pragma once
#include <string>
#include <vector>
using namespace std;

enum class Drive_type
{
	UNKNOWN,
	HARDDRIVE,
	ROM,
	REMOVABLE,
	NETWORK
};

struct Drive_info
{
	wstring name;
	wstring description;
	uint64_t size;
	Drive_type type;
	bool isMounted;
};

struct File_data
{
	wstring name;
	bool is_directory;
};

extern const vector<Drive_info> get_drives();
extern const vector<File_data> list_directory(const wstring& directory);
