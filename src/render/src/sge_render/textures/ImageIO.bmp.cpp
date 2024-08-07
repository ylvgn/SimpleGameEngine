#pragma once

#include "ImageIO.bmp.h"

namespace sge {

#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

struct BITMAPCOREHEADER { // 12 bytes
// Windows 2.0 or later
// BMP Version 1.x (IBM OS/2 1.x)
	u32 bcSize;
	u16 bcWidth;
	u16 bcHeight;
	u16 bcPlanes;
	u16 bcBitCount; /*aka bpp*/
};
typedef BITMAPCOREHEADER OS21XBITMAPHEADER;

struct OS22XBITMAPHEADER { // 64 bytes
// BMP Version 2.x (IBM OS/2 2.x)
	u32  Size;             /* Size of this structure in bytes */
	u32  Width;            /* Bitmap width in pixels */
	u32  Height;           /* Bitmap height in pixel */
	u16  NumPlanes;        /* Number of bit planes (color depth) */
	u16  BitsPerPixel;     /* Number of bits per pixel per plane */
	/* Fields added for OS/2 2.x follow this line */
	u32  Compression;      /* Bitmap compression scheme */
	u32  ImageDataSize;    /* Size of bitmap data in bytes */
	u32  XResolution;      /* X resolution of display device */
	u32  YResolution;      /* Y resolution of display device */
	u32  ColorsUsed;       /* Number of color table indices used */
	u32  ColorsImportant;  /* Number of important color indices */
	u16  Units;            /* Type of units used to measure resolution */
	u16  Reserved;         /* Pad structure to 4-byte boundary */
	u16  Recording;        /* Recording algorithm */
	u16  Rendering;        /* Halftoning algorithm used */
	u32  Size1;            /* Reserved for halftoning algorithm use */
	u32  Size2;            /* Reserved for halftoning algorithm use */
	u32  ColorEncoding;    /* Color model used in bitmap */
	u32  Identifier;       /* Reserved for application use */
};
typedef OS22XBITMAPHEADER BITMAPCOREHEADER2;

struct BITMAPINFOHEADER { // 40 bytes
// Windows NT, 3.1x or later
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

//BITMAPV2HEADER / BITMAPV2INFOHEADER(52 bytes) Undocumented

struct BITMAPV3HEADER { // 56 bytes
// Not officially documented
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
	/* Fields added for V3 follow this line */
	u32 biRedMask;
	u32 biGreenMask;
	u32 biBlueMask;
	u32 biAlphaMask;
};
typedef BITMAPV3HEADER BITMAPV3INFOHEADER;

typedef u32 FXPT2DOT30;
struct CIEXYZ {
	FXPT2DOT30 ciexyzX;
	FXPT2DOT30 ciexyzY;
	FXPT2DOT30 ciexyzZ;
};
struct CIEXYZTRIPLE {
	CIEXYZ  ciexyzRed;
	CIEXYZ  ciexyzGreen;
	CIEXYZ  ciexyzBlue;
};
struct BITMAPV4HEADER { // 108 bytes
// Windows NT 4.0, 95 or later
	u32				bV4Size;
	i32				bV4Width;
	i32				bV4Height;
	u16				bV4Planes;
	u16				bV4BitCount;
	u32				bV4V4Compression;
	u32				bV4SizeImage;
	i32				bV4XPelsPerMeter;
	i32				bV4YPelsPerMeter;
	u32				bV4ClrUsed;
	u32				bV4ClrImportant;
	u32				bV4RedMask;
	u32				bV4GreenMask;
	u32				bV4BlueMask;
	u32				bV4AlphaMask;
	u32				bV4CSType;
	CIEXYZTRIPLE	bV4Endpoints;
	u32				bV4GammaRed;
	u32				bV4GammaGreen;
	u32				bV4GammaBlue;
};

struct BITMAPV5HEADER { // 124 bytes
// Windows NT 5.0, 98 or later
	u32				bV5Size;
	i32				bV5Width;
	i32				bV5Height;
	i16				bV5Planes;
	i16				bV5BitCount;
	u32				bV5Compression;
	u32				bV5SizeImage;
	i32				bV5XPelsPerMeter;
	i32				bV5YPelsPerMeter;
	u32				bV5ClrUsed;
	u32				bV5ClrImportant;
	u32				bV5RedMask;
	u32				bV5GreenMask;
	u32				bV5BlueMask;
	u32				bV5AlphaMask;
	u32				bV5CSType;
	CIEXYZTRIPLE	bV5Endpoints;
	u32				bV5GammaRed;
	u32				bV5GammaGreen;
	u32				bV5GammaBlue;
	u32				bV5Intent;
	u32				bV5ProfileData;
	u32				bV5ProfileSize;
	u32				bV5Reserved;
};

void ImageIO_bmp::Reader::load(Image& img, ByteSpan data, ColorType expectType) {
	BinDeserializer de(data);
	BMPInfo info;

	u8 sign;
	de.io_fixed_le(sign);
	if (sign != 'B')
		throw SGE_ERROR("Erros BMP file signature: B");
	de.io_fixed_le(sign);
	if (sign != 'M')
		throw SGE_ERROR("Erros BMP file signature: M");

	de.advance(sizeof(u32)); // 4 bytes: The size of the BMP filesize in bytes
	de.advance(sizeof(u16)); // 2 bytes: Reserved
	de.advance(sizeof(u16)); // 2 bytes: Reserved

	de.io_fixed_le(info.offset);
	if (!info.offset)
		throw SGE_ERROR("Erros BMP file startingOffset == 0");

	de.io_fixed_le(info.cbSize);
	switch (info.cbSize) {
		case sizeof(BITMAPCOREHEADER): { // 12
			BITMAPCOREHEADER hdr;
			_readHeader(de, hdr);

			info.width	= hdr.bcWidth;
			info.height = hdr.bcHeight;
			info.bpp	= hdr.bcBitCount;
		} break;
		case sizeof(OS22XBITMAPHEADER): { // 64
			OS22XBITMAPHEADER hdr;
			_readHeader(de, hdr);

			info.width		= hdr.Width;
			info.height		= hdr.Height;
			info.bpp		= hdr.BitsPerPixel;
			info.compress	= hdr.Compression;
		} break;
		case sizeof(BITMAPINFOHEADER): { // 40
			BITMAPINFOHEADER hdr;
			_readHeader(de, hdr);

			info.width		= hdr.biWidth;
			info.height		= hdr.biHeight;
			info.bpp		= hdr.biBitCount;
			info.compress	= hdr.biCompression;
		} break;
		case sizeof(BITMAPV3HEADER): { // 56
			BITMAPV3HEADER hdr;
			_readHeader(de, hdr);

			info.width		= hdr.biWidth;
			info.height		= hdr.biHeight;
			info.bpp		= hdr.biBitCount;
			info.compress	= hdr.biCompression;
			info.maskR		= hdr.biRedMask;
			info.maskG		= hdr.biGreenMask;
			info.maskB		= hdr.biBlueMask;
			info.maskA		= hdr.biAlphaMask;
		} break;
		case sizeof(BITMAPV4HEADER): { // 108
			BITMAPV4HEADER hdr;
			_readHeader(de, hdr);

			info.width		= hdr.bV4Width;
			info.height		= hdr.bV4Height;
			info.bpp		= hdr.bV4BitCount;
			info.compress	= hdr.bV4V4Compression;
			info.maskR		= hdr.bV4RedMask;
			info.maskG		= hdr.bV4GreenMask;
			info.maskB		= hdr.bV4BlueMask;
			info.maskA		= hdr.bV4AlphaMask;
		} break;
		case sizeof(BITMAPV5HEADER): { // 124
			BITMAPV5HEADER hdr;
			_readHeader(de, hdr);

			info.width		= hdr.bV5Width;
			info.height		= hdr.bV5Height;
			info.bpp		= hdr.bV5BitCount;
			info.compress	= hdr.bV5Compression;
			info.maskR		= hdr.bV5RedMask;
			info.maskG		= hdr.bV5GreenMask;
			info.maskB		= hdr.bV5BlueMask;
			info.maskA		= hdr.bV5AlphaMask;
		} break;

		default:
			throw SGE_ERROR("Erros BMP file unknown type");
	}

	if (info.cbSize != sizeof(BITMAPCOREHEADER)) {
		auto& compress = info.compress;
		if (compress == 1 || compress == 2)
			throw SGE_ERROR("bmp unsupported RLE");
		if (compress >= 4)
			throw SGE_ERROR("bmp jpeg,png mode unsupported compression");
		if (compress == 3 && info.bpp != 16 && info.bpp != 32)
			throw SGE_ERROR("bmp bitfields requires 16 or 32 bpp");
	}

	SGE_ASSERT(data.begin() + info.offset == de.cur());
	ByteSpan remainSpan(de.cur(), de.remain());

	auto& width  = info.width;
	auto& height = info.width;

	switch (info.bpp) {
		case 24: info.colorType = ColorType::RGBb; break;
		case 32: info.colorType = ColorType::RGBAb; break;
		default: throw SGE_ERROR("bmp error unsupported format");
	}

	size_t totalSize = width * height * ColorUtil::pixelSizeInBytes(info.colorType);
	ByteSpan srcSpan(remainSpan.begin(), totalSize);

	if (expectType == ColorType::None) {
		_readPixelData(img, info, srcSpan);
	}
	else if (expectType == ColorType::RGBAb) {
		using DST = Color4b;
		using DST_ElementType = DST::ElementType;

		switch (info.colorType) {
			case ColorType::RGBAb: _readPixelData(img, info, srcSpan); break;
			case ColorType::RGBAs: _readPixelDataFromRGBorRGBA<DST, ColorRGBA<u16>>(img, info, srcSpan); break;
			case ColorType::RGBAf: _readPixelDataFromRGBorRGBA<DST, ColorRGBA<f32>>(img, info, srcSpan); break;

			case ColorType::RGBb:  _readPixelDataFromRGBorRGBA<DST, ColorRGB<u8> >(img, info, srcSpan, 0); break;
			case ColorType::RGBs:  _readPixelDataFromRGBorRGBA<DST, ColorRGB<u16>>(img, info, srcSpan, 255); break;
			case ColorType::RGBf:  _readPixelDataFromRGBorRGBA<DST, ColorRGB<f32>>(img, info, srcSpan, 255); break;

			default: SGE_ERROR("bmp error unsupported format");
		}
	}
	else if (expectType == ColorType::RGBb) {
		using DST = ColorRGB<u8>;
		switch (info.colorType) {
			case ColorType::RGBb:  _readPixelData(img, info, srcSpan); break;
			case ColorType::RGBs:  _readPixelDataFromRGBorRGBA<DST, ColorRGB<u16>>(img, info, srcSpan);	break;
			case ColorType::RGBf:  _readPixelDataFromRGBorRGBA<DST, ColorRGB<f32>>(img, info, srcSpan);	break;

			case ColorType::RGBAb: _readPixelDataFromRGBorRGBA<DST, ColorRGBA<u8> >(img, info, srcSpan); break;
			case ColorType::RGBAs: _readPixelDataFromRGBorRGBA<DST, ColorRGBA<u16>>(img, info, srcSpan); break;
			case ColorType::RGBAf: _readPixelDataFromRGBorRGBA<DST, ColorRGBA<f32>>(img, info, srcSpan); break;
			default: SGE_ERROR("bmp error unsupported format");
		}
	}
	else
		throw SGE_ERROR("bmp error unsupported format");
}

void ImageIO_bmp::Reader::_readPixelData(Image& out, const BMPInfo& info, ByteSpan srcPixelData) {
	SGE_ASSERT(info.colorType != ColorType::None);
	out.create(info.colorType, info.width, info.height);
	out.copyToPixelData(srcPixelData);
	_swapBGRAToRGBA(out);
}

void ImageIO_bmp::Reader::_swapBGRAToRGBA(Image& out) {
	switch (out.colorType()) {
		case ColorType::RGBb:	_swapColorChannel< ColorRGB<u8> >(out, 0, 2); break;
		case ColorType::RGBs:	_swapColorChannel< ColorRGB<u16>>(out, 0, 2); break;
		case ColorType::RGBf:	_swapColorChannel< ColorRGB<f32>>(out, 0, 2); break;

		case ColorType::RGBAb:	_swapColorChannel< ColorRGBAb	>(out, 0, 2); break;
		case ColorType::RGBAs:	_swapColorChannel< ColorRGBAs	>(out, 0, 2); break;
		case ColorType::RGBAf:	_swapColorChannel< ColorRGBAf	>(out, 0, 2); break;
	}
}

void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, BITMAPCOREHEADER& hdr) {
	hdr.bcSize = sizeof(hdr);
	de.io_fixed_le(hdr.bcWidth);
	de.io_fixed_le(hdr.bcHeight);
	de.io_fixed_le(hdr.bcPlanes);
	_throwIfInvalidNumPlanes(hdr.bcPlanes);
	de.io_fixed_le(hdr.bcBitCount);
}

void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, OS22XBITMAPHEADER& hdr) {
	hdr.Size = sizeof(hdr);
	de.io_fixed_le(hdr.Width);
	de.io_fixed_le(hdr.Height);
	de.io_fixed_le(hdr.NumPlanes);
	_throwIfInvalidNumPlanes(hdr.NumPlanes);
	de.io_fixed_le(hdr.BitsPerPixel);
	de.io_fixed_le(hdr.Compression);
	de.io_fixed_le(hdr.ImageDataSize);
	de.io_fixed_le(hdr.XResolution);
	
}
void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, BITMAPINFOHEADER& hdr) {
	hdr.biSize = sizeof(hdr);
	de.io_fixed_le(hdr.biWidth);
	de.io_fixed_le(hdr.biHeight);
	de.io_fixed_le(hdr.biPlanes);
	_throwIfInvalidNumPlanes(hdr.biPlanes);
	de.io_fixed_le(hdr.biBitCount);
	de.io_fixed_le(hdr.biCompression);
	de.io_fixed_le(hdr.biSizeImage);
	de.io_fixed_le(hdr.biXPelsPerMeter);
	de.io_fixed_le(hdr.biYPelsPerMeter);
	de.io_fixed_le(hdr.biClrUsed);
	de.io_fixed_le(hdr.biClrImportant);

}
void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, BITMAPV3HEADER& hdr) {
	hdr.biSize = sizeof(hdr);
	de.io_fixed_le(hdr.biWidth);
	de.io_fixed_le(hdr.biHeight);
	de.io_fixed_le(hdr.biPlanes);
	_throwIfInvalidNumPlanes(hdr.biPlanes);
	de.io_fixed_le(hdr.biBitCount);
	de.io_fixed_le(hdr.biCompression);
	de.io_fixed_le(hdr.biSizeImage);
	de.io_fixed_le(hdr.biXPelsPerMeter);
	de.io_fixed_le(hdr.biYPelsPerMeter);
	de.io_fixed_le(hdr.biClrUsed);
	de.io_fixed_le(hdr.biClrImportant);
	de.io_fixed_le(hdr.biRedMask);
	de.io_fixed_le(hdr.biGreenMask);
	de.io_fixed_le(hdr.biBlueMask);
	de.io_fixed_le(hdr.biAlphaMask);
}
void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, BITMAPV4HEADER& hdr) {
	hdr.bV4Size = sizeof(hdr);
	de.io_fixed_le(hdr.bV4Width);
	de.io_fixed_le(hdr.bV4Height);
	de.io_fixed_le(hdr.bV4Planes);
	_throwIfInvalidNumPlanes(hdr.bV4Planes);
	de.io_fixed_le(hdr.bV4BitCount);
	de.io_fixed_le(hdr.bV4V4Compression);
	de.io_fixed_le(hdr.bV4SizeImage);
	de.io_fixed_le(hdr.bV4XPelsPerMeter);
	de.io_fixed_le(hdr.bV4YPelsPerMeter);
	de.io_fixed_le(hdr.bV4ClrUsed);
	de.io_fixed_le(hdr.bV4ClrImportant);
	de.io_fixed_le(hdr.bV4RedMask);
	de.io_fixed_le(hdr.bV4GreenMask);
	de.io_fixed_le(hdr.bV4BlueMask);
	de.io_fixed_le(hdr.bV4AlphaMask);
	de.io_fixed_le(hdr.bV4CSType);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzRed.ciexyzX);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzRed.ciexyzY);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzRed.ciexyzZ);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzGreen.ciexyzX);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzGreen.ciexyzY);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzGreen.ciexyzZ);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzBlue.ciexyzX);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzBlue.ciexyzY);
	de.io_fixed_le(hdr.bV4Endpoints.ciexyzBlue.ciexyzZ);
	de.io_fixed_le(hdr.bV4GammaRed);
	de.io_fixed_le(hdr.bV4GammaGreen);
	de.io_fixed_le(hdr.bV4GammaBlue);
}

void ImageIO_bmp::Reader::_readHeader(BinDeserializer& de, BITMAPV5HEADER& hdr) {
	hdr.bV5Size = sizeof(hdr);
	de.io_fixed_le(hdr.bV5Width);
	de.io_fixed_le(hdr.bV5Height);
	de.io_fixed_le(hdr.bV5Planes);
	_throwIfInvalidNumPlanes(hdr.bV5Planes);
	de.io_fixed_le(hdr.bV5BitCount);
	de.io_fixed_le(hdr.bV5Compression);
	de.io_fixed_le(hdr.bV5SizeImage);
	de.io_fixed_le(hdr.bV5XPelsPerMeter);
	de.io_fixed_le(hdr.bV5YPelsPerMeter);
	de.io_fixed_le(hdr.bV5ClrUsed);
	de.io_fixed_le(hdr.bV5ClrImportant);
	de.io_fixed_le(hdr.bV5RedMask);
	de.io_fixed_le(hdr.bV5GreenMask);
	de.io_fixed_le(hdr.bV5BlueMask);
	de.io_fixed_le(hdr.bV5AlphaMask);
	de.io_fixed_le(hdr.bV5CSType);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzRed.ciexyzX);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzRed.ciexyzY);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzRed.ciexyzZ);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzGreen.ciexyzX);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzGreen.ciexyzY);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzGreen.ciexyzZ);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzBlue.ciexyzX);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzBlue.ciexyzY);
	de.io_fixed_le(hdr.bV5Endpoints.ciexyzBlue.ciexyzZ);
	de.io_fixed_le(hdr.bV5GammaRed);
	de.io_fixed_le(hdr.bV5GammaGreen);
	de.io_fixed_le(hdr.bV5GammaBlue);
	de.io_fixed_le(hdr.bV5Intent);
	de.io_fixed_le(hdr.bV5ProfileData);
	de.io_fixed_le(hdr.bV5ProfileSize);
	de.io_fixed_le(hdr.bV5Reserved);
}

} //namespace
