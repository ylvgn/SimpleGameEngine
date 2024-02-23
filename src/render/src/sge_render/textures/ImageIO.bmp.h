#pragma once

#include "Image.h"

namespace sge {

struct BITMAPINFOHEADER;

struct ImageIO_bmp {

	class Reader {
	public:
		void load(Image& img, ByteSpan data, ColorType expectType = ColorType::None);

	private:
		void _readHeader(BinDeserializer& de, BITMAPINFOHEADER& hdr);
	};
};

} //namespace
