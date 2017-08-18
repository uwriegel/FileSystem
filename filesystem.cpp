#include <Windows.h>
#include <array>
#include "filesystem.h"
#include "tools.h"
using namespace std;

const wstring get_drive_description(const wstring& name);
Drive_type get_drive_type(unsigned int type);
bool is_mounted(const file_handle& volume);
uint64_t get_volume_size(const wstring& directory_on_drive);
void get_data(const WIN32_FIND_DATAW& find_data, back_insert_iterator<vector<Item>>& it);
template <typename O>
void list_files(const wstring& directory, O iter);
wstring get_directory_parent(const wstring& path);
bool hasEnding(wstring const &fullString, wstring const &ending);
wstring get_url(const wstring& filename);

const vector<Drive_info> get_drives()
{
	array<wchar_t, 500> buffer;
	auto size = GetLogicalDriveStringsW(static_cast<DWORD>(buffer.size()), buffer.data());
	wstring drive_string(buffer.data(), size);
	auto drives = split(drive_string, 0);

	vector<Drive_info> drive_infos;
	transform(drives.begin(), drives.end(), back_inserter(drive_infos), [](const wstring& val) {
		auto type = GetDriveTypeW(val.c_str());
		auto volume = wstring{ L"\\\\.\\" + val.substr(0, 2) };
		file_handle volume_handle(CreateFileW(volume.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			nullptr, OPEN_EXISTING, 0, nullptr));
		return move(Drive_info
		{
			val,
			move(get_drive_description(val)),
			get_volume_size(val),
			get_drive_type(type),
			type != 3 ? is_mounted(volume_handle) : true
		});
	});

	auto erase_it = remove_if(drive_infos.begin(), drive_infos.end(), [](const Drive_info& val) {
		return !val.isMounted;
	});
	drive_infos.erase(erase_it, drive_infos.end());

	return move(drive_infos);
}

const vector<Item> list_files(const wstring& directory)
{
	vector<Item> result;
	list_files(directory, back_inserter(result));

	sort(result.begin(), result.end(), [](const auto& lvalue, const auto& rvalue) {
		return lvalue.is_directory > rvalue.is_directory;
	});
	return move(result);
}

wstring get_normalized_path(wstring directory)
{
	auto path = get_full_path_name(directory);
	if (path[path.length() - 1] == '\\')
		path.resize(path.length() - 1);
	return move(path);
}

wstring get_full_path_name(const wstring& path)
{
	array<wchar_t, 1000> buffer;
	auto size = GetFullPathNameW(path.c_str(), static_cast<DWORD>(buffer.size()), buffer.data(), nullptr);
	return move(wstring(buffer.data(), size));
}

wstring get_url(const wstring& filename)
{
	if (!hasEnding(filename, L".exe"s))
	{
		auto pos = filename.rfind('.');
		if (pos != string::npos)
			return L"http://localhost:20000/Commander/Icon?file="s + filename.substr(pos);
	}
	return L"http://localhost:20000/Commander/Icon?file="s + filename;
}

const wstring get_drive_description(const wstring& name)
{
	array<wchar_t, 400> buffer;
	if (GetVolumeInformationW(name.c_str(), buffer.data(), static_cast<DWORD>(buffer.size()), nullptr, nullptr, nullptr, nullptr, 0))
		return wstring(buffer.data(), wcslen(buffer.data()));
	else
		return wstring();
}

uint64_t get_volume_size(const wstring& directory_on_drive)
{
	ULARGE_INTEGER result{ 0 };
	GetDiskFreeSpaceExW(directory_on_drive.c_str(), nullptr, &result, nullptr);
	return result.QuadPart;
}

Drive_type get_drive_type(unsigned int type)
{
	switch (type)
	{
		case 2:
			return Drive_type::REMOVABLE;
		case 3:
			return Drive_type::HARDDRIVE;
		case 4:
			return Drive_type::NETWORK;
		case 5:
			return Drive_type::ROM;
		default:
			return Drive_type::UNKNOWN;
	}
}

bool is_mounted(const file_handle& volume)
{
	if (volume == INVALID_HANDLE_VALUE)
		// 2 means "no disk", anything else means by inference "disk
		// in drive, but you do not have admin privs to do a
		// CreateFile on that volume".
		return GetLastError() != 2;

	DWORD bytes_returned; // ignored
	auto result = DeviceIoControl(volume, FSCTL_IS_VOLUME_MOUNTED, nullptr, 0, nullptr, 0, &bytes_returned, nullptr);
	return result != 0;
}

template <typename O>
void list_files(const wstring& directory, O iter) 
{
	WIN32_FIND_DATAW find_data{ 0 };
	auto search_path = wstring { directory + L"\\*.*" };
	auto handle = FindFirstFileW(search_path.c_str(), &find_data);
	auto aua = GetLastError();
	if (handle == INVALID_HANDLE_VALUE)
		return;

	*iter++ = { 
		L"images/parentfolder.png",
		L"",
		static_cast<uint64_t>(-1L),
		L"",
		Item_kind::PARENT,
		false,
		true,
		get_directory_parent(directory)
	};
	get_data(find_data, iter);

	while (true)
	{
		if (!FindNextFileW(handle, &find_data))
		{
			FindClose(handle);
			break;
		}
		get_data(find_data, iter);
	}
}

void get_data(const WIN32_FIND_DATAW& find_data, back_insert_iterator<vector<Item>>& it)
{
	if (!showHidden && ((find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN))
		return;
	wstring filename{ find_data.cFileName };
	if (filename == L"." || filename == L"..")
		return;

	auto is_directory = (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
	*it++ = move(Item{
		is_directory ? L"images/Folder.png" : move(get_url(filename)),
		move(filename),
		static_cast<uint64_t>((find_data.nFileSizeHigh * (MAXDWORD + 1)) + find_data.nFileSizeLow),
		format_time(find_data.ftLastWriteTime),
		is_directory ? Item_kind::DIRECTORY : Item_kind::FILE,
		(find_data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN,
		is_directory,
		L""
	});
}

wstring get_directory_parent(const wstring& path)
{
	if (path.length() == 2)
		return L"drives";
	auto pos = path.rfind('\\');
	if (pos == wstring::npos)
		pos = 0;
	auto result(path);
	result.resize(pos);
	if (result.length() == 2)
		result.append(L"\\");
	return result;
}

bool hasEnding(wstring const &fullString, wstring const &ending) 
{
	if (fullString.length() >= ending.length()) 
		return (fullString.compare(fullString.length() - ending.length(), ending.length(), ending) == 0);
	else 
		return false;
}

bool showHidden{ false };