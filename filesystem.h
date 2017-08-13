#pragma once
#include <string>
#include <vector>

enum class Drive_type
{
	UNKNOWN,
	REMOVABLE,
	HARDDRIVE,
	ROM
};

struct Drive_info
{
	std::wstring name;
	std::wstring description;
	Drive_type type;
	bool isMounted;
};

extern const std::vector<Drive_info> get_drives();
