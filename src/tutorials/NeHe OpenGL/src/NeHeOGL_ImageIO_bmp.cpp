#include "NeHeOGL_ImageIO_bmp.h"

#include "externals/stb_image.h"

namespace sge {

NeHeOGL_ImageIO_bmp::Reader::~Reader() {
	if (_uc) {
		stbi_image_free(_uc);
		_uc = nullptr;
	}
}

int NeHeOGL_ImageIO_bmp::Reader::s_onRead(void* io_user_data, char* dest, int len) {
	auto* thisObj = static_cast<Reader*>(io_user_data);
	if (!thisObj)
		throw SGE_ERROR("bmp error s_onRead");
	return thisObj->onRead(dest, len);
}

void NeHeOGL_ImageIO_bmp::Reader::s_onSkip(void* io_user_data, int len) {
	auto* thisObj = static_cast<Reader*>(io_user_data);
	if (!thisObj)
		throw SGE_ERROR("bmp error s_onSkip");
	thisObj->onSkip(len);
}

int NeHeOGL_ImageIO_bmp::Reader::s_onEOF(void* io_user_data) {
	auto* thisObj = static_cast<Reader*>(io_user_data);
	if (!thisObj)
		throw SGE_ERROR("bmp error s_onEOF");
	return thisObj->onEOF();
}

int NeHeOGL_ImageIO_bmp::Reader::onRead(char* dest, int len) {
	// stbi_uc buffer_start[128];
	ptrdiff_t n = len;
	if (_readPtr + n > _data.end())
		n = _data.end() - _readPtr; // len is stbi__context data member 'buffer_start' buffer size, it always 128
	if (_readPtr == _data.end())
		throw SGE_ERROR("bmp error read data out of range");
	memcpy(dest, _readPtr, n);
	_readPtr += n;
	return static_cast<int>(n);
}

void NeHeOGL_ImageIO_bmp::Reader::onSkip(int len) {
	if (_readPtr + len > _data.end())
		throw SGE_ERROR("bmp error skip data out of range");
	_readPtr += len;
}

int NeHeOGL_ImageIO_bmp::Reader::onEOF() {
	return _readPtr == _data.end();
}

void NeHeOGL_ImageIO_bmp::Reader::load(NeHeOGL_Image& img, ByteSpan data, ColorType expectType) {
	_data			= data;
	_readPtr		= data.begin();
	int dataSize	= static_cast<int>(_data.size());

	int in_width;
	int in_height;
	int out_req_comp = STBI_default;

	if (expectType == ColorType::None) {
		if (!stbi_info_from_memory(_readPtr, dataSize, &in_width, &in_height, &out_req_comp))
			throw SGE_ERROR("bmp error read info: {}", stbi_failure_reason());
	} else {
		switch (expectType) {
			case ColorType::RGBb:	out_req_comp = STBI_rgb; break;
			case ColorType::RGBAb:	out_req_comp = STBI_rgb_alpha; break;
			default: throw SGE_ERROR("bmp error unsupported format");
		}
	}

	SGE_ASSERT(out_req_comp != STBI_default);

	auto out_color_type = ColorType::None;

	switch (out_req_comp) {
		case STBI_rgb:			out_color_type = ColorType::RGBb;	break;
		case STBI_rgb_alpha:	out_color_type = ColorType::RGBAb;	break;
		default: throw SGE_ERROR("bmp error unsupported format");
	}

#if 1
	stbi_io_callbacks clbk;
	clbk.read	= s_onRead;
	clbk.skip	= s_onSkip;
	clbk.eof	= s_onEOF;
	_uc			= stbi_load_from_callbacks(&clbk, this, &in_width, &in_height, nullptr, out_req_comp);
#else
	_uc			= stbi_load_from_memory(_readPtr, dataSize, &in_width, &in_height, nullptr, out_req_comp); // use this line is also work.
#endif

	if (!_uc)
		throw SGE_ERROR("bmp error read pixels: {}", stbi_failure_reason());

	SGE_ASSERT(out_color_type != ColorType::None);

	int width				= in_width;
	int height				= in_height;
	int strideInBytes		= width * ColorUtil::pixelSizeInBytes(out_color_type);
	size_t dataSizeInBytes	= height * strideInBytes;

	Span<stbi_uc> src(_uc, dataSizeInBytes);
	ByteSpan dst = ByteSpan_make(src);

	img.create(out_color_type, width, height, strideInBytes, 1, dataSizeInBytes);
	img.copyToPixelData(dst);
}

}