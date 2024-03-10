#pragma once

#include "Image.h"

namespace sge {

struct BITMAPCOREHEADER;
struct OS22XBITMAPHEADER;
struct BITMAPINFOHEADER;
//struct BITMAPV2HEADER; // Undocumented
struct BITMAPV3HEADER;
struct BITMAPV4HEADER;
struct BITMAPV5HEADER;

struct ImageIO_bmp {

	class Reader {
	public:
		void load(Image& img, ByteSpan data, ColorType expectType = ColorType::None);

	private:
		struct BMPInfo {
			u32 offset	= 0;
			u32 cbSize	= 0;
			u32 width	= 0;
			u32 height	= 0;

			u32 bpp		= 0;
		};

		void _readHeader(BinDeserializer& de, BITMAPCOREHEADER& hdr);
		void _readHeader(BinDeserializer& de, OS22XBITMAPHEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPINFOHEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPV3HEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPV4HEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPV5HEADER& hdr);

		void throwIfInvalidNumPlanes(int numPlanes) {
			if (numPlanes != 1)
				throw SGE_ERROR("bmp error: The number of planes for the target device, it must be 1");
		}
	};
};

} //namespace
