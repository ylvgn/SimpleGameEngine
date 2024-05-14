#include "NeHeOGL_ImageIO_png.h"

namespace sge {

NeHeOGL_ImageIO_png::Reader::~Reader() {
	if (!_info) {
		png_destroy_info_struct(_png, &_info);
		_info = nullptr;
	}
	if (_png) {
		png_destroy_read_struct(&_png, (png_infopp)NULL, (png_infopp)NULL);
		_png = nullptr;
	}
}

bool NeHeOGL_ImageIO_png::Reader::error_longjmp_restore_point() {
	// !!! call this function before any libpng C-function that might longjmp()
	// to avoid any C++ destructor or exception try/catch block happen in between

	#if SGE_COMPILER_VC
		#pragma warning(push) 
		#pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
	#endif

	// longjmp() to here from pngReportError()
	return setjmp(png_jmpbuf(_png)) != 0;

	#if SGE_COMPILER_VC
		#pragma warning(pop) 
	#endif
}

void NeHeOGL_ImageIO_png::Reader::load(NeHeOGL_Image& img, ByteSpan data, ColorType expectType) {
	_data = data;
	_readPtr = data.data();

	if (data.size() < 8 || png_sig_cmp(data.data(), 0, 8)) {
		// https://www.w3.org/TR/PNG-Rationale.html#R.PNG-file-signature
		throw SGE_ERROR("not png format");
	}

	_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!_png)
		throw SGE_ERROR("png_create_read_struct");

	png_set_error_fn(_png, this, s_onError, s_onWarning); // redirect stderr to my own logger function

	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_create_info_struct");
	}

	_info = png_create_info_struct(_png);
	if (!_info) {
		png_destroy_read_struct(&_png, (png_infopp)NULL, (png_infopp)NULL);
		throw SGE_ERROR("png_destroy_read_struct");
	}

	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_set_alpha_mode");
	}

	if (img.isPreMulAlpha())
		png_set_alpha_mode(_png, PNG_ALPHA_PREMULTIPLIED, PNG_DEFAULT_sRGB);
	else
		png_set_alpha_mode(_png, PNG_ALPHA_PNG, PNG_DEFAULT_sRGB); // default

	png_set_read_fn(_png, this, s_onRead);

//	If you had previously opened the file and read any of the signature from the beginning in order to see if this was a PNG file,
//	you need to let libpng know that there are some bytes missing from the start of the file.
	//	png_set_sig_bytes(png_ptr, number);

	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_read_info");
	}

	png_read_info(_png, _info);

	png_uint_32 in_width;
	png_uint_32 in_height;
	int			in_bit_depth;
	int			in_color_type;
	int			in_interlace_type;

	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_get_IHDR");
	}

	png_get_IHDR(_png, _info,
				 &in_width,
				 &in_height,
				 &in_bit_depth,			// holds the bit depth of one of the image channels
				 &in_color_type,		// describes which color/alpha channels are present
				 &in_interlace_type,	// PNG_INTERLACE_NONE or PNG_INTERLACE_ADAM7
				 nullptr, nullptr);
	
	auto out_color_type = ColorType::None;
	bool in_palette_has_alpha = false;

/*
	PNG_COLOR_TYPE_GRAY (bit depths 1, 2, 4, 8, 16)
	PNG_COLOR_TYPE_GRAY_ALPHA (bit depths 8, 16)
	PNG_COLOR_TYPE_PALETTE (bit depths 1, 2, 4, 8)
	PNG_COLOR_TYPE_RGB (bit_depths 8, 16)
	PNG_COLOR_TYPE_RGB_ALPHA (bit_depths 8, 16)
*/

	if (in_bit_depth < 8) {
		// PNG files pack pixels of bit depths 1, 2, and 4 into bytes
		SGE_LOG_WARN("png file bit depth < 8");
		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_set_packing");
		}
		png_set_packing(_png);
		in_bit_depth = 8;
	}

	if (expectType == ColorType::None) {
		if (in_bit_depth == 8) {
			switch (in_color_type) {
				case PNG_COLOR_TYPE_GRAY:		out_color_type = ColorType::Lb; break;
				case PNG_COLOR_TYPE_GRAY_ALPHA: out_color_type = ColorType::LAb; break;
				case PNG_COLOR_TYPE_RGB:		out_color_type = ColorType::RGBAb; break;
				case PNG_COLOR_TYPE_RGB_ALPHA:	out_color_type = ColorType::RGBAb; break;
				case PNG_COLOR_TYPE_PALETTE: {
					png_bytep		in_trans_alpha	= nullptr;
					int				in_num_trans	= 0;
					png_color_16p	in_trans_values = nullptr;

					if (error_longjmp_restore_point()) {
						throw SGE_ERROR("png error png_get_tRNS");
					}
					// https://github.com/winlibs/libpng/blob/7d8a246298423743e66ac72a8c58f032fb4e96a8/pngget.c#L1092
					png_get_tRNS(_png, _info, &in_trans_alpha, &in_num_trans, &in_trans_values);

					out_color_type = ColorType::RGBAb;

					if (in_trans_alpha) {
						in_palette_has_alpha = true;
//						out_color_type = ColorType::RGBAb;
					} else {
//						out_color_type = ColorType::RGBb; // DX Not Support
					}
				} break;
			}
		}
		else if (in_bit_depth == 16) {
			switch (in_color_type) {
				case PNG_COLOR_TYPE_GRAY:		out_color_type = ColorType::Ls; break;
				case PNG_COLOR_TYPE_GRAY_ALPHA: out_color_type = ColorType::LAs; break;
				case PNG_COLOR_TYPE_RGB:		out_color_type = ColorType::RGBAs; break;
				case PNG_COLOR_TYPE_RGB_ALPHA:	out_color_type = ColorType::RGBAs; break;
			}
		}
		else {
			throw SGE_ERROR("png error unsupported format");
		}
	}
	
	setReadFilter(out_color_type, in_color_type, in_bit_depth, in_palette_has_alpha);

	int width	= 0;
	int height	= 0;
	width		= in_width;
	height		= in_height;

	SGE_ASSERT(out_color_type != ColorType::None);
	img.create(out_color_type, width, height);

	Vector<png_bytep, 2048> row_pointers; //row_pointers = png_malloc(png_ptr, height * sizeof(png_bytep));
	row_pointers.resize(height);

	for (int y = 0; y < height; y++) {
		row_pointers[y] = static_cast<png_bytep>(img.rowBytes(y).data());
	}

	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_set_rows");
	}
	png_set_rows(_png, _info, row_pointers.data());

#if 1
	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_read_image");
	}
	png_read_image(_png, row_pointers.data());
#else
	png_uint_32 number_of_rows = static_cast<png_uint_32>(row_pointers.size());
	// If you don't want to read in the whole image at once, you can use png_read_rows

	if (error_longjmp_restore_point()) {
		throw SGE_ERROR("png error png_read_rows");
	}
	png_read_rows(_png, row_pointers.data(), nullptr, number_of_rows); // 'sparkle'
#endif
}

void NeHeOGL_ImageIO_png::Reader::setReadFilter(ColorType out_color_type, int in_color_type, int in_bit_depth, bool in_palette_has_alpha) {
//	http://www.libpng.org/pub/png/libpng-1.2.5-manual.html#section-3.7

	auto elementType = ColorUtil::elementType(out_color_type);
	if (elementType == ColorElementType::None)
		throw SGE_ERROR("png error unsupported format");

// 8 <-> 16 bit
	switch (elementType) {
		case ColorElementType::UNorm8: {
			switch (in_bit_depth) {
				case 8:	break;
				case 16: {
					if (error_longjmp_restore_point()) {
						throw SGE_ERROR("png error png_set_scale_16");
					}
					png_set_scale_16(_png);
				} break;
				default: throw SGE_ERROR("png error unsupported format");
			}
		} break;

		case ColorElementType::UNorm16: {
			switch (in_bit_depth) {
				case 8: {
					if (error_longjmp_restore_point()) {
						throw SGE_ERROR("png error png_set_expand_16");
					}
					png_set_expand_16(_png);
				} break;
				case 16: break;
				default: throw SGE_ERROR("png error unsupported format");
			}
			#if SGE_CPU_ENDIAN_LITTLE // aware more than 1 byte
				if (error_longjmp_restore_point()) {
					throw SGE_ERROR("png error png_set_swap");
				}
				png_set_swap(_png);
			#endif
		} break;

		default: throw SGE_ERROR("png error unsupported format");
	}

// palette
	if (in_color_type == PNG_COLOR_TYPE_PALETTE) {
		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_set_palette_to_rgb");
		}
		png_set_palette_to_rgb(_png);
	}
		

// alpha channel
	if (ColorUtil::hasAlpha(out_color_type)) {
		// add alpha channel if missing
		if (in_color_type & PNG_COLOR_MASK_PALETTE) {
			if (!in_palette_has_alpha) { // palette no alpha channer, but we need alpha
				if (error_longjmp_restore_point()) {
					throw SGE_ERROR("png error png_set_filler");
				}
				png_set_filler(_png, 0xffffffff, PNG_FILLER_AFTER);
			}
		}
		if (!(in_color_type & PNG_COLOR_MASK_ALPHA)) { // no alpha channel, but we need alpha
			if (error_longjmp_restore_point()) {
				throw SGE_ERROR("png error png_set_filler");
			}
			png_set_filler(_png, 0xffffffff, PNG_FILLER_AFTER);
		}
	} else { // we dont want alpha channel
		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_set_strip_alpha");
		}
		png_set_strip_alpha(_png);
	}

// RGB <-> Grayscale
	auto out_color_model = ColorUtil::colorModel(out_color_type);
	switch (out_color_model) {
		case ColorModel::RGB:
		case ColorModel::RGBA: {
			if (!(in_color_type & PNG_COLOR_MASK_COLOR)) { // no rgb channel, but we need rgb
				if (error_longjmp_restore_point()) {
					throw SGE_ERROR("png error png_set_gray_to_rgb");
				}
				png_set_gray_to_rgb(_png);
			}
		} break;

		case ColorModel::L:
		case ColorModel::LA: {
			if (in_color_type & PNG_COLOR_MASK_COLOR) { // exist rgb channel, but we dont want rgb
				if (error_longjmp_restore_point()) {
					throw SGE_ERROR("png error png_set_rgb_to_gray_fixed");
				}
				png_set_rgb_to_gray_fixed(_png, 1, -1, -1);
			}
		} break;

		default: throw SGE_ERROR("png error unsupported format");
	}
}

void NeHeOGL_ImageIO_png::Reader::s_onError(png_structp png_ptr, png_const_charp error_msg) {
	auto* p = reinterpret_cast<Reader* const>(png_get_error_ptr(png_ptr));
	if (!p) {
		throw SGE_ERROR("s_onError png_get_error_ptr");
	}
	p->onError(error_msg);
}

void NeHeOGL_ImageIO_png::Reader::onError(png_const_charp error_msg) {
	SGE_LOG_ERROR("libpng error: {}", error_msg);
}

void NeHeOGL_ImageIO_png::Reader::s_onWarning(png_structp png_ptr, png_const_charp warning_msg) {
	auto* p = reinterpret_cast<Reader* const>(png_get_error_ptr(png_ptr));
	if (!p) {
		throw SGE_ERROR("s_onWarning png_get_error_ptr");
	}
	p->onWarning(warning_msg);
}

void NeHeOGL_ImageIO_png::Reader::onWarning(png_const_charp warning_msg) {
	SGE_LOG_WARN("libpng warning: {}", warning_msg);
}

void NeHeOGL_ImageIO_png::Reader::s_onRead(png_structp png_ptr, png_bytep dest, png_size_t length) {
	 auto* p = reinterpret_cast<Reader* const>(png_get_io_ptr(png_ptr));
	 if (!p) {
		 throw SGE_ERROR("s_onRead png_get_io_ptr");
	 }
	 p->onRead(dest, length);
}

void NeHeOGL_ImageIO_png::Reader::onRead(png_bytep dest, png_size_t length) {
	// https://github.com/torch/image/blob/5aa18819b6a7b44751f8a858bd232d1c07b67985/png.c#L33
	size_t n = length;
	if (_readPtr + n > _data.end()) {
		throw SGE_ERROR("png error read data out of range");
	}
	memcpy(dest, _readPtr, n);
	_readPtr += n;
}

}