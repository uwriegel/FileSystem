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


