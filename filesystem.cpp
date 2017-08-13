#include <Windows.h>
#include <array>
#include "filesystem.h"
#include "tools.h"
using namespace std;

const wstring get_drive_description(const wstring& name);
Drive_type get_drive_type(unsigned int type);
bool is_mounted(const wstring& drive);

const vector<Drive_info> get_drives()
{
	array<wchar_t, 500> buffer;
	auto size = GetLogicalDriveStringsW(buffer.size(), buffer.data());
	wstring driveString(buffer.data(), size);
	auto drives = split(driveString, 0);

	vector<Drive_info> driveInfos;
	transform(drives.begin(), drives.end(), back_inserter(driveInfos), [](const wstring& val) {
		auto type = GetDriveTypeW(val.c_str());
		return move(Drive_info
		{
			val,
			move(get_drive_description(val)),
			get_drive_type(type),
			type != 3 ? is_mounted(val) : true
		});
	});

	auto erase_it = remove_if(driveInfos.begin(), driveInfos.end(), [](const Drive_info& val) {
		return !val.isMounted;
	});
	driveInfos.erase(erase_it, driveInfos.end());

	return move(driveInfos);
}

const wstring get_drive_description(const wstring& name)
{
	array<wchar_t, 400> buffer;
	if (GetVolumeInformationW(name.c_str(), buffer.data(), buffer.size(), nullptr, nullptr, nullptr, nullptr, 0))
		return wstring(buffer.data(), wcslen(buffer.data()));
	else
		return wstring();
}

Drive_type get_drive_type(unsigned int type)
{
	switch (type)
	{
		case 2:
			return Drive_type::REMOVABLE;
		case 3:
			return Drive_type::HARDDRIVE;
		case 5:
			return Drive_type::ROM;
		default:
			return Drive_type::UNKNOWN;
	}
}

bool is_mounted(const wstring& drive)
{
	wstring volume{ L"\\\\.\\" + drive.substr(0, 2) };

	auto handle = CreateFileW(volume.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		nullptr, OPEN_EXISTING, 0, nullptr);

	if (handle == INVALID_HANDLE_VALUE)
		// 2 means "no disk", anything else means by inference "disk
		// in drive, but you do not have admin privs to do a
		// CreateFile on that volume".
		return GetLastError() != 2;

	DWORD bytesReturned; // ignored
	auto devSuccess = DeviceIoControl(handle, FSCTL_IS_VOLUME_MOUNTED, nullptr, 0, nullptr, 0, &bytesReturned, nullptr);
	return devSuccess != 0;
}
