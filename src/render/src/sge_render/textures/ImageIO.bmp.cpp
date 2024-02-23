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

	if (hdr.biCompression == BI_RGB) {
		colorType = ColorType::RGBAb; // is it wrong ???
		SGE_DUMP_VAR(hdr.biBitCount);
	}

	if (colorType == ColorType::None) {
		throw SGE_ERROR("bmp error unsupported output format");
	}

	img.create(colorType, hdr.biWidth, hdr.biHeight);
	img.copyToPixelData(ByteSpan(de.cur(), de.remain()));
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
