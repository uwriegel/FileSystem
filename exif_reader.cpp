#include <winsock2.h>
#include "exif_reader.h"

Exif_reader::Exif_reader(const wstring& path)
	: exif_stream(path, ofstream::binary)
{
}

Exif_reader::~Exif_reader()
{}

bool Exif_reader::initialize()
{
	if (!exif_stream)
		return false;
	if (read_ushort() != 0xFFD8)
		//throw new Exception("File is not a valid JPEG");
		return false;
	if (!read_to_exif_start())
		return false;
	
	// The next 4 bytes are the size of the Exif data.
	auto length = read_ushort();
	vector<char> bytes(length);
	exif_stream.read(bytes.data(), bytes.size());

	// Next is the Exif data itself. It starts with the ASCII "Exif" followed by 2 zero bytes.
	string exif(bytes.data(), 4);
	if (exif != "Exif")
		return false;

	// 2 zero bytes
	if (bytes[4] != 0 || bytes[5] != 0)
		//throw new Exception("Malformed Exif data");
		return false;

	// We're now into the TIFF format
	auto tiff_header_start = 6;

	// What byte align will be used for the TIFF part of the document? II for Intel, MM for Motorola
	string ii(bytes.data() + 6, 2);
	is_little_endian = ii == "II";

	// Next 2 bytes are always the same.
	if (read_ushort(bytes, 8) != 0x002A)
		//throw new Exception("Error in TIFF data");
		return false;

	// Get the offset to the IFD (image file directory)
	uint32_t ifd_offset = read_uint(bytes, 10);

	// Note that this offset is from the first byte of the TIFF header. Jump to the IFD.
	auto position = ifd_offset + tiff_header_start;
	check_ifd(bytes, position);
	return true;
}

bool Exif_reader::read_to_exif_start()
{
	// The file has a number of blocks (Exif/JFIF), each of which
	// has a tag number followed by a length. We scan the document until the required tag (0xFFE1)
	// is found. All tags start with FF, so a non FF tag indicates an error.

	// Get the next tag.
	uint8_t marker_start;
	uint8_t marker_number{ 0 };

	while (((marker_start = read_byte()) == 0xFF) && (marker_number = read_byte()) != 0xE1)
	{
		// Get the length of the data.
		auto data_length = read_ushort();
		// Jump to the end of the data (note that the size field includes its own size)!
		exif_stream.seekg(data_length - 2, istream::cur);
	}

	// It's only success if we found the 0xFFE1 marker
	if (marker_start != 0xFF || marker_number != 0xE1)
		//throw new Exception("Could not find Exif data block");
		return false;
	return true;
}

void Exif_reader::check_ifd(const vector<char>& bytes, int start_position)
{
	auto count = read_ushort(bytes, start_position);
}

uint8_t Exif_reader::read_byte()
{
	char ret{ 0 };
	exif_stream.read(&ret, 1);
	return static_cast<uint8_t>(ret);
}

uint16_t Exif_reader::read_ushort()
{
	uint16_t ret{ 0 };
	exif_stream.read(reinterpret_cast<char*>(&ret), 2);
	return htons(ret);
}

uint16_t Exif_reader::read_ushort(const vector<char>& buffer, int offset)
{
	uint16_t ret;
	memcpy(&ret, buffer.data() + offset, 2);
	return is_little_endian? ret : htons(ret);
}

uint32_t Exif_reader::read_uint(const vector<char>& buffer, int offset)
{
	uint32_t ret;
	memcpy(&ret, buffer.data() + offset, 4);
	return is_little_endian ? ret : htons(ret);
}