#pragma once

// http://www.libpng.org/pub/png/libpng-1.2.5-manual.html
#include "libpng16/png.h"

#include "NeHeOGL_Image.h"

namespace sge {

struct NeHeOGL_ImageIO_png {

	struct Reader {
		~Reader();

		void load(NeHeOGL_Image& img, ByteSpan data, ColorType expectType = ColorType::None);

		static void s_onError(png_structp png_ptr, png_const_charp error_msg);
		void onError(png_const_charp error_msg);

		static void s_onWarning(png_structp png_ptr, png_const_charp warning_msg);
		void onWarning(png_const_charp warning_msg);

		static void s_onRead(png_structp png_ptr, png_bytep dest, png_size_t length);
		void onRead(png_bytep dest, png_size_t length);

		void setReadFilter(ColorType out_color_type, int in_color_type, int in_bit_depth, bool in_palette_has_alpha);

		png_structp _png	= nullptr;
		png_infop	_info	= nullptr;

		ByteSpan	_data;
		const u8*	_readPtr;
	};

};

}