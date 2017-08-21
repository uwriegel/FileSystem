#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class Exif_reader 
{
public:
	Exif_reader(const wstring& path);
	~Exif_reader();

	bool initialize();
private:
	ifstream exif_stream;

	bool read_to_exif_start();
	uint8_t read_byte();
	uint16_t read_ushort();

	uint16_t read_ushort(const vector<char>& buffer, int offset);
	uint32_t read_uint(const vector<char>& buffer, int offset);

	void check_ifd(const vector<char>& bytes, int start_position);

	bool is_little_endian{ false };
};
