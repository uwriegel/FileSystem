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

enum class Item_kind
{
	DRIVE,
	PARENT,
	FILE,
	DIRECTORY,
	SERVICE,
	REGISTRY,
	REGISTRY_PROPERTY
};

struct Drive_info
{
	wstring name;
	wstring description;
	uint64_t size;
	Drive_type type;
	bool isMounted;
};

struct Item
{
	wstring image_url;
	wstring name;
	uint64_t size;
	wstring date_time;
	Item_kind kind;
	bool is_hidden;
	bool is_directory;
	wstring parent;
};

extern const vector<Drive_info> get_drives();
extern const vector<Item> list_files(const wstring& directory);
extern wstring get_full_path_name(const wstring& path);
extern wstring get_normalized_path(wstring directory);
extern wstring get_directory_parent(const wstring& path);
extern bool showHidden;
