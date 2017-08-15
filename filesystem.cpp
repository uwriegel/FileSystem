#include <Windows.h>
#include <array>
#include "filesystem.h"
#include "tools.h"
using namespace std;

const wstring get_drive_description(const wstring& name);
Drive_type get_drive_type(unsigned int type);
bool is_mounted(const file_handle& volume);
uint64_t get_volume_size(const wstring& directory_on_drive);
File_data get_data(const WIN32_FIND_DATAW& find_data);

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

	*iter++ = move(get_data(find_data));

	while (true)
	{
		if (!FindNextFileW(handle, &find_data))
		{
			FindClose(handle);
			break;
		}
		*iter++ = move(get_data(find_data));
	}
}

const vector<File_data> list_directory(const wstring& directory)
{
	vector<File_data> result;
	list_files(directory, back_inserter(result));

	sort(result.begin(), result.end(), [](const auto& lvalue, const auto& rvalue) {
		return lvalue.is_directory > rvalue.is_directory;
	});
	return move(result);
}

File_data get_data(const WIN32_FIND_DATAW& find_data)
{
	return File_data{ find_data.cFileName, (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY };
}
