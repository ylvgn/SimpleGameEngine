#pragma once

#include "Color.h"

namespace sge {

struct ColorUtil {
	ColorUtil() = delete;

	static constexpr int pixelSizeInBytes(ColorType v);
	static constexpr int bytesPerPixelBlock(ColorType type);
	static constexpr int bytesPerPixelBlockImageSize(int width, int height, ColorType type);

	static constexpr ColorElementType	elementType(ColorType v) { return static_cast<ColorElementType>(enumInt(v) & 0xff); }
	static constexpr ColorModel			colorModel (ColorType v) { return static_cast<ColorModel>((enumInt(v) >> 8) & 0xff); }

	static constexpr bool isCompressedType(ColorType v) { return colorModel(v) == ColorModel::BlockCompression; }

	static constexpr bool hasAlpha(ColorType v);
}; // ColorUtil

constexpr
bool ColorUtil::hasAlpha(ColorType v) {
	auto model = colorModel(v);
	switch (model) {
		case ColorModel::RGBA: return true;
	}
	return false;
}

constexpr
int ColorUtil::pixelSizeInBytes(ColorType v) {
	switch (v) {
		case ColorType::RGBb:	return sizeof(ColorRGB<u8> );
		case ColorType::RGBs:	return sizeof(ColorRGB<u16>);
		case ColorType::RGBf:	return sizeof(ColorRGB<f32>);

		case ColorType::RGBAb:	return sizeof(ColorRGBAb);
		case ColorType::RGBAs:	return sizeof(ColorRGBAs);
		case ColorType::RGBAf:	return sizeof(ColorRGBAf);

		case ColorType::Rb:		return sizeof(ColorRb);
		case ColorType::Rs:		return sizeof(ColorRs);
		case ColorType::Rf:		return sizeof(ColorRf);

		case ColorType::Lb:		return sizeof(ColorLb);
		case ColorType::Ls:		return sizeof(ColorLs);
		case ColorType::Lf:		return sizeof(ColorLf);
	}

	SGE_ASSERT(false);
	return 0;
}

constexpr
int ColorUtil::bytesPerPixelBlock(ColorType type) {
	switch (type) {
		#define E(T) \
			case ColorType::T: return Color##T::kBytesPerPixelBlock;
		//----------
			E(BC1)
			E(BC2)
			E(BC3)
			E(BC4)
			E(BC5)
			E(BC6h)
			E(BC7)
		#undef E
	}
	return 0;
}

constexpr
int ColorUtil::bytesPerPixelBlockImageSize(int width, int height, ColorType type) {
	// e.g. BC7 2592x1080 px -> ((2592+3)/4) * ((1080+3)/4) * 16(bytes) / 1024 / 1024 = 2.6696 MB

	auto blockSize = bytesPerPixelBlock(type);
	return ((width+3)/4) * ((height+3)/4) * blockSize;
}

} // namespace sge