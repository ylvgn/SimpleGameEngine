#include "MyTexture2D.h"
#include "libpng16/png.h"

namespace sge {

void MyTexture2D::create(int width, int height, Color4b* pixels) {
	destroy();

	if (!my_is_pow2(width) || !my_is_pow2(height))
		throw SGE_ERROR("width or height must be power of 2");

	glGenTextures(1, &_tex);
	glBindTexture(GL_TEXTURE_2D, _tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	_width  = width;
	_height = height;
}

void MyTexture2D::destroy() {
	if (_tex) {
		glDeleteTextures(1, &_tex);
		_tex = 0;
	}
}

void MyTexture2D::bind() const {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void MyTexture2D::unbind() const {
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

class PngReader : public NonCopyable {
public:
	~PngReader() { destroy(); }

	void destroy() {
		if (_png) {
			png_destroy_read_struct(&_png, &_info, nullptr);
			_png  = nullptr;
			_info = nullptr;
		}
	}

	bool error_longjmp_restore_point() {
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

	static void _onRead(png_structp png_ptr, png_bytep data, png_size_t length) {
		auto* p = reinterpret_cast<PngReader*>(png_get_io_ptr(png_ptr));
		p->onRead(data, length);
	}

	void onRead(png_bytep dst, png_size_t length) {
		size_t n = length;
		if (_readPtr + n > _data.end())
			throw SGE_ERROR("png error read data out of range");

		memcpy(dst, _readPtr, n);
		_readPtr += n;
	}

	void loadFile(StrView filename, bool mulAlpha) {
		MemMapFile mm;
		mm.open(filename);
		loadMem(mm.span(), filename, mulAlpha);
	}

	void loadMem(ByteSpan data, StrView filename, bool mulAlpha) {
		destroy();

		_data = data;
		_readPtr = _data.begin();

		_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if (!_png)
			throw SGE_ERROR("png_create_read_struct");

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_create_info_struct");
		}

		_info = png_create_info_struct(_png);
		if (!_info)
			throw SGE_ERROR("png_create_info_struct");

		png_set_read_fn(_png, this, _onRead);

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_read_info");
		}
		png_read_info(_png, _info);

		png_uint_32 w, h;
		int bit;
		int colorType;
		int interlace;
		int compression;
		int filter;

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_get_IHDR");
		}

		png_get_IHDR(_png, _info, &w, &h, &bit, &colorType, &interlace, &compression, &filter);

		_width = w;
		_height = h;

		switch (colorType) {
			case PNG_COLOR_TYPE_RGB_ALPHA: {
			}break;

			case PNG_COLOR_TYPE_RGB: {
				png_set_filler(_png, 255, PNG_FILLER_AFTER);
			}break;

			case PNG_COLOR_TYPE_GRAY: {
				png_set_gray_to_rgb(_png);
				png_set_filler(_png, 255, PNG_FILLER_AFTER);
			}break;

			case PNG_COLOR_TYPE_GRAY_ALPHA: {
				png_set_gray_to_rgb(_png);
			}break;

			case PNG_COLOR_TYPE_PALETTE: {
				png_set_palette_to_rgb(_png);

				bool hasAlpha = false;

				{
					//get palette transparency table
					png_bytep		trans_alpha = nullptr;
					int				num_trans   = 0;
					png_color_16p	trans_color = nullptr;
					png_get_tRNS(_png, _info, &trans_alpha, &num_trans, &trans_color);
					if (trans_alpha)
						hasAlpha = true;
				}

				if (!hasAlpha)
					png_set_filler(_png, 255, PNG_FILLER_AFTER);
			}break;
		}

		_pixels.resize(w * h);

		Vector<u8*> rows;
		rows.resize(h);

		for (size_t i = 0; i < h; i++) {
			rows[i] = _pixels[i * w].data;
		}

		if (error_longjmp_restore_point()) {
			throw SGE_ERROR("png error png_read_image");
		}
		png_read_image(_png, rows.data());

		if (mulAlpha) {
			for (auto& p : _pixels) {
				 p.r = static_cast<u8>(static_cast<int>(p.r) * p.a / 255);
				 p.g = static_cast<u8>(static_cast<int>(p.g) * p.a / 255);
				 p.b = static_cast<u8>(static_cast<int>(p.b) * p.a / 255);
			}
		}
	}

	int width () const { return _width; }
	int height() const { return _height; }

			Span<Color4b>		pixels()		{ return _pixels.span(); }
	const	Span<const Color4b> pixels() const	{ return _pixels.span(); }

	Color4b* pixels_ptr() { return _pixels.data(); }

private:
	png_structp	_png  = nullptr;
	png_infop	_info = nullptr;

	int _width  = 0;
	int _height = 0;

	Vector<Color4b> _pixels;

	ByteSpan		_data;
	const u8*		_readPtr = nullptr;
};

void MyTexture2D::loadPngFile(StrView filename, bool mulAlpha) {
	PngReader r;
	r.loadFile(filename, mulAlpha);

	create(r.width(), r.height(), r.pixels_ptr());
}

}