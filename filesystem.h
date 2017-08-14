#pragma once
#include <string>
#include <vector>

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
	std::wstring name;
	std::wstring description;
	uint64_t size;
	Drive_type type;
	bool isMounted;
};

extern const std::vector<Drive_info> get_drives();


