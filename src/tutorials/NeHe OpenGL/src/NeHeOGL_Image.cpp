#include "NeHeOGL_Image.h"
#include "NeHeOGL_ImageIO_bmp.h"
#include "NeHeOGL_ImageIO_png.h"

namespace sge {

NeHeOGL_Image::NeHeOGL_Image(NeHeOGL_Image && r) noexcept {
	_pixelData.clear();
	_info = r._info;
	_pixelData.swap(r._pixelData);

	r._info = NeHeOGL_Image::Info();
}

void NeHeOGL_Image::clear() {
	_info = NeHeOGL_Image::Info();
	_pixelData.clear();
}

void NeHeOGL_Image::loadFile(StrView filename) {
	auto ext = FilePath::extension(filename);
	if (0 == StringUtil::ignoreCaseCompare(ext, "bmp")) {
		return loadBmpFile(filename);
	}
	if (0 == StringUtil::ignoreCaseCompare(ext, "png")) {
		return loadPngFile(filename);
	}
	throw SGE_ERROR("unsupported image file format {}", ext);
}

void NeHeOGL_Image::loadBmpFile(StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadBmpMem(mm);
}

void NeHeOGL_Image::loadBmpMem(ByteSpan data) {
	NeHeOGL_ImageIO_bmp::Reader r;
	r.load(*this, data, ColorType::RGBAb);
}

void NeHeOGL_Image::loadPngFile(StrView filename) {
	MemMapFile mm;
	mm.open(filename);
	loadPngMem(mm);
}

void NeHeOGL_Image::loadPngMem(ByteSpan data) {
	NeHeOGL_ImageIO_png::Reader r;
	r.load(*this, data);
}

void NeHeOGL_Image::create(ColorType colorType, int width, int height) {
	create(colorType, width, height, width * ColorUtil::pixelSizeInBytes(colorType));
}

void NeHeOGL_Image::create(ColorType colorType, int width, int height, size_t strideInBytes) {
	_create(colorType, width, height, strideInBytes, 1, strideInBytes * height);
}

void NeHeOGL_Image::create(ColorType colorType, int width, int height, size_t strideInBytes, int mipmapCount, size_t dataSizeInBytes) {
	_create(colorType, width, height, strideInBytes, mipmapCount, dataSizeInBytes);
}

void NeHeOGL_Image::_create(ColorType colorType, int width, int height, size_t strideInBytes, int mipmapCount, size_t dataSizeInBytes) {
	_info.colorType = colorType;
	_info.size.set(width, height);
	_info.strideInBytes = strideInBytes;

	try {
		_pixelData.clear();
		_pixelData.resize(dataSizeInBytes);
	}
	catch (...) {
		clear();
		throw;
	}
}

}