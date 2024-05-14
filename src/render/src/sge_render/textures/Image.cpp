#include "Image.h"
#include "ImageIO_png.h"
#include "ImageIO.dds.h"
#include "ImageIO.bmp.h"

namespace sge {

void Image::clear() {
	_info = ImageInfo();
	_pixelData.clear();
}

void Image::loadFile(StrView filename, ColorType expectType /*= ColorType::None*/) {
	auto ext = FilePath::extension(filename);
	if (0 == StringUtil::ignoreCaseCompare(ext, "png")) {
		return loadPngFile(filename, expectType);
	}

	if (0 == StringUtil::ignoreCaseCompare(ext, "dds")) {
		return loadDdsFile(filename, expectType);
	}

	if (0 == StringUtil::ignoreCaseCompare(ext, "bmp")) {
		return loadBmpFile(filename, expectType);
	}

	throw SGE_ERROR("unsupported image file format {}", ext);
}

void Image::loadPngFile(StrView filename, ColorType expectType /*= ColorType::None*/)
{
	MemMapFile mm;
	mm.open(filename);
	loadPngMem(mm, expectType);
}

void Image::loadPngMem(ByteSpan data, ColorType expectType /*= ColorType::None*/) {
	ImageIO_png::Reader r;
	r.load(*this, data, expectType);
}

void Image::loadDdsFile(StrView filename, ColorType expectType /*= ColorType::None*/)
{
	MemMapFile mm;
	mm.open(filename);
	loadDdsMem(mm, expectType);
}

void Image::loadDdsMem(ByteSpan data, ColorType expectType /*= ColorType::None*/) {
	ImageIO_dds::Reader r;
	r.load(*this, data, expectType);
}

void Image::loadBmpFile(StrView filename, ColorType expectType /*= ColorType::None*/) {
	MemMapFile mm;
	mm.open(filename);
	loadBmpMem(mm, expectType);
}

void Image::loadBmpMem(ByteSpan data, ColorType expectType /*= ColorType::None*/) {
	ImageIO_bmp::Reader r;
	r.load(*this, data, expectType);
}

void Image::create(ColorType colorType, int width, int height) {
	create(colorType, width, height, width * ColorUtil::pixelSizeInBytes(colorType));
}

void Image::create(ColorType colorType, int width, int height, int strideInBytes) {
	_create(colorType, width, height, strideInBytes, 1, strideInBytes * height);
}

void Image::create(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes) {
	_create(colorType, width, height, strideInBytes, mipmapCount, dataSizeInBytes);
}

void Image::_create(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes) {
	_info.colorType = colorType;
	_info.size.set(width, height);
	_info.strideInBytes = strideInBytes;
	_info.mipmapCount = mipmapCount;

	try {
		_pixelData.clear();
		_pixelData.resize(dataSizeInBytes);
	}
	catch (...) {
		clear();
		throw;
	}
}

} // namespace