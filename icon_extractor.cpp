#include <Windows.h>
#include <Gdiplus.h>
#include <memory>
#include <vector>
#include <string>
#include "icon_extractor.h"
#include "memory_stream.h"
using namespace std;
using namespace Gdiplus;

struct BITMAP_AND_PIXELS
{
	BITMAP_AND_PIXELS(int number_of_bits)
		: pixels(number_of_bits) {}
	unique_ptr<Bitmap> bmp;
	vector<int> pixels;
};

CLSID png_clsid{ 0 };
ULONG_PTR gdiplus_token{0};

CLSID get_encoder_clsid(const wstring& format);
BITMAP_AND_PIXELS create_alpha_channel_bitmap_from_icon(HICON hIcon);

void gdiplus_initialize()
{
	GdiplusStartupInput gdiplus_startup_input;
	gdiplus_token;
	auto status = GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, nullptr);

	png_clsid = get_encoder_clsid(L"image/png"s);
}

void gdiplus_uninitialize()
{
	GdiplusShutdown(gdiplus_token);
}

const vector<char> extract_icon(const wstring& icon_path)
{
	SHFILEINFOW file_info{ 0 };
	SHGetFileInfoW(icon_path.c_str(), FILE_ATTRIBUTE_NORMAL, &file_info, sizeof(file_info),
		SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME);
	auto icon = file_info.hIcon;
	auto result = create_alpha_channel_bitmap_from_icon(icon);
	Memory_stream ms;
	result.bmp->Save(&ms, &png_clsid);
	auto bytes = ms.get_bytes();
	DestroyIcon(icon);
	return bytes;
}

CLSID get_encoder_clsid(const wstring& format)
{
	UINT num{ 0 };
	UINT  size{ 0 };
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return { 0 };

	vector<char> image_codec_info_buffer(size);
	auto image_codec_info = reinterpret_cast<ImageCodecInfo*>(image_codec_info_buffer.data());
	GetImageEncoders(num, size, image_codec_info);
	for (auto i = 0u; i < num; ++i)
		if (format == image_codec_info[i].MimeType)
			return image_codec_info[i].Clsid;
	return { 0 };
}

BITMAP_AND_PIXELS create_alpha_channel_bitmap_from_icon(HICON icon) 
{
	ICONINFO icon_info{ 0 };
	GetIconInfo(icon, &icon_info);

	auto dc = GetDC(nullptr);

	BITMAP bm{ 0 };
	GetObject(icon_info.hbmColor, sizeof(bm), &bm);

	BITMAPINFO bmi{ 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = bm.bmWidth;
	bmi.bmiHeader.biHeight = -bm.bmHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	int number_of_bits = bm.bmWidth * bm.bmHeight;
	
	BITMAP_AND_PIXELS result(number_of_bits);
	GetDIBits(dc, icon_info.hbmColor, 0, bm.bmHeight, result.pixels.data(), &bmi, DIB_RGB_COLORS);

	// Check whether the color bitmap has an alpha channel.
	// (On my Windows 7, all file icons I tried have an alpha channel.)
	auto has_alpha{ false };
	for (int i = 0; i < number_of_bits; i++) 
		if ((result.pixels[i] & 0xff000000) != 0) {
			has_alpha = TRUE;
			break;
		}

	// If no alpha values available, apply the mask bitmap
	if (!has_alpha) 
	{
		// Extract the mask bitmap
		vector<int> mask_bits(number_of_bits);
		GetDIBits(dc, icon_info.hbmMask, 0, bm.bmHeight, mask_bits.data(), &bmi, DIB_RGB_COLORS);
		// Copy the mask alphas into the color bits
		for (int i = 0; i < number_of_bits; i++) 
			if (mask_bits[i] == 0) 
				result.pixels[i] |= 0xff000000;
	}

	ReleaseDC(nullptr, dc);
	DeleteObject(icon_info.hbmColor);
	DeleteObject(icon_info.hbmMask);

	// Create GDI+ Bitmap
	result.bmp = make_unique<Bitmap>(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppARGB, reinterpret_cast<BYTE*>(result.pixels.data()));

	return result;
}
//
//static HICON getShellIconByIndex(int shilsize, int iImage)
//{
//	IImageListPtr spiml;
//	SHGetImageList(shilsize, IID_PPV_ARGS(&spiml));
//
//	HICON hico;
//	spiml->GetIcon(iImage, ILD_TRANSPARENT, &hico);
//	return hico;
//}
//
//static HICON getShellIcon(int shilsize, const std::wstring& fname) {
//	UINT flags = SHGFI_SYSICONINDEX;
//	SHFILEINFO fi = { 0 };
//	HICON hIcon = NULL;
//
//	if (SHGetFileInfo(fname.c_str(), 0, &fi, sizeof(fi), flags) != 0) {
//		hIcon = getShellIconByIndex(shilsize, fi.iIcon);
//	}
//
//	return hIcon;
//}
//
//struct BITMAP_AND_BYTES {
//	Gdiplus::Bitmap* bmp;
//	int32_t* bytes;
//};
//
//static BITMAP_AND_BYTES createAlphaChannelBitmapFromIcon(HICON hIcon) {
//
//	// Get the icon info
//	ICONINFO iconInfo = { 0 };
//	GetIconInfo(hIcon, &iconInfo);
//
//	// Get the screen DC
//	HDC dc = GetDC(NULL);
//
//	// Get icon size info
//	BITMAP bm = { 0 };
//	GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
//
//	// Set up BITMAPINFO
//	BITMAPINFO bmi = { 0 };
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biWidth = bm.bmWidth;
//	bmi.bmiHeader.biHeight = -bm.bmHeight;
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biBitCount = 32;
//	bmi.bmiHeader.biCompression = BI_RGB;
//
//	// Extract the color bitmap
//	int nBits = bm.bmWidth * bm.bmHeight;
//	int32_t* colorBits = new int32_t[nBits];
//	GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);
//
//	// Check whether the color bitmap has an alpha channel.
//	// (On my Windows 7, all file icons I tried have an alpha channel.)
//	BOOL hasAlpha = FALSE;
//	for (int i = 0; i < nBits; i++) {
//		if ((colorBits[i] & 0xff000000) != 0) {
//			hasAlpha = TRUE;
//			break;
//		}
//	}
//
//	// If no alpha values available, apply the mask bitmap
//	if (!hasAlpha) {
//		// Extract the mask bitmap
//		int32_t* maskBits = new int32_t[nBits];
//		GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
//		// Copy the mask alphas into the color bits
//		for (int i = 0; i < nBits; i++) {
//			if (maskBits[i] == 0) {
//				colorBits[i] |= 0xff000000;
//			}
//		}
//		delete[] maskBits;
//	}
//
//	// Release DC and GDI bitmaps
//	ReleaseDC(NULL, dc);
//	::DeleteObject(iconInfo.hbmColor);
//	::DeleteObject(iconInfo.hbmMask);
//
//	// Create GDI+ Bitmap
//	Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppARGB, (BYTE*)colorBits);
//	BITMAP_AND_BYTES ret = { bmp, colorBits };
//
//	return ret;
//}
//
//static void saveFileIconAsPng(int shilsize, const std::wstring& fname, const std::wstring& pngFile) {
//	HICON hIcon = getShellIcon(shilsize, fname);
//	BITMAP_AND_BYTES bbs = createAlphaChannelBitmapFromIcon(hIcon);
//
//	IStream* fstrm = NULL;
//	SHCreateStreamOnFile(pngFile.c_str(), STGM_WRITE | STGM_CREATE, &fstrm);
//	bbs.bmp->Save(fstrm, &g_pngClsid, NULL);
//	fstrm->Release();
//
//	delete bbs.bmp;
//	delete[] bbs.bytes;
//	DestroyIcon(hIcon);
//}