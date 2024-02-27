#pragma once

#include "ImageIO.bmp.h"

namespace sge {

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

// Windows NT, 3.1x or later
struct BITMAPINFOHEADER {
	u32 biSize;
	i32	biWidth;
	i32	biHeight;
	u16	biPlanes;
	u16	biBitCount;
	u32	biCompression;
	u32	biSizeImage;
	i32	biXPelsPerMeter;
	i32	biYPelsPerMeter;
	u32	biClrUsed;
	u32	biClrImportant;
};

void ImageIO_bmp::Reader::load(Image& img, ByteSpan data, ColorType expectType) {
	expectType = ColorType::RGBAb; // tmp

	BinDeserializer de(data);

	// read 2 bytes 'BM'
	uint8_t sign;
	de.io_fixed_le(sign);
		if (sign != 'B') throw SGE_ERROR("Erros BMP file signature: B");
	de.io_fixed_le(sign);
		if (sign != 'M') throw SGE_ERROR("Erros BMP file signature: M");
	
	BITMAPINFOHEADER hdr;
	_readHeader(de, hdr);

	auto colorType = ColorType::None;
	auto bpp = hdr.biBitCount;
	if (bpp == 24) {
		colorType = ColorType::RGBb;
	} else if(bpp == 32) {
		colorType = ColorType::RGBAb;
	}

	ByteSpan src(de.cur(), de.cur() + de.remain() - 2); // maybe wrong??
	ByteSpan dst;

	Vector<u8> pixelData;
	if (colorType == ColorType::RGBb) {
		pixelData.resize(src.size() / 3 * 4);
		auto* p = pixelData.data();
		auto* q = src.begin();
		for (;q != src.end();) {
			*p = *q; p++; q++; // b
			*p = *q; p++; q++; // g
			*p = *q; p++; q++; // r
			*p = 255; p++;     // a

			swap(*(p - 2), *(p - 4));
		}
	} else {
		pixelData.assign(src.begin(), src.end());
		for (int i = 0; i < pixelData.size(); i+=4) {
			swap(pixelData[i], pixelData[i + 2]); // bgra -> rgba
		}
	}
	dst = ByteSpan_make(pixelData.span());;

	colorType = expectType;

	int strideInBytes = hdr.biWidth * ColorUtil::pixelSizeInBytes(colorType);
	img.create(colorType, hdr.biWidth, hdr.biHeight, strideInBytes);
	img.copyToPixelData(dst);
}

void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, BITMAPINFOHEADER& hdr) {
	// 4 bytes: The size of the BMP file in bytes (.bmp filesize)
	// 2 bytes: Reserved
	// 2 bytes: Reserved
	de.advance(4 + 2 + 2);

	u32 startingOffset = 0;
	de.io_fixed_le(startingOffset);
	de.io_fixed_le(hdr.biSize);
	de.io_fixed_le(hdr.biWidth);
	de.io_fixed_le(hdr.biHeight);
	de.io_fixed_le(hdr.biPlanes);
	de.io_fixed_le(hdr.biBitCount);
	de.io_fixed_le(hdr.biCompression);
	de.io_fixed_le(hdr.biSizeImage);
	de.io_fixed_le(hdr.biXPelsPerMeter);
	de.io_fixed_le(hdr.biYPelsPerMeter);
	de.io_fixed_le(hdr.biClrUsed);
	de.io_fixed_le(hdr.biClrImportant);

	if (hdr.biSize != sizeof(BITMAPINFOHEADER)) {
		throw SGE_ERROR("BMP invalid header size");
	}
}

} //namespace
