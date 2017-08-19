#pragma once
#include <vector>
using namespace std;

extern void gdiplus_initialize();
extern void gdiplus_uninitialize();
extern const vector<char> extract_icon(const wstring& icon_path);
