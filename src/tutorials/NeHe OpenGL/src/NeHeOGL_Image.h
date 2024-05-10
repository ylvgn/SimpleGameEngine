#pragma once

namespace sge {
	
struct NeHeOGL_ImageInfo {
	NeHeOGL_ImageInfo()
		: size({0,0})
		, strideInBytes(0)
		, mipmapCount(1)
		, colorType(ColorType::None)
		, isPreMulAlpha(false)
	{}

	Vec2i		size			{ 0,0 };
	size_t		strideInBytes	= 0;
	int			mipmapCount		= 1;
	ColorType	colorType		= ColorType::None;

	bool		isPreMulAlpha : 1;

	int			pixelSizeInBytes() const { return ColorUtil::pixelSizeInBytes(colorType); }
};

class NeHeOGL_Image : public NonCopyable {
public:
	using Info = NeHeOGL_ImageInfo;

	NeHeOGL_Image() = default;
	NeHeOGL_Image(NeHeOGL_Image && r) noexcept;

	void clear();

	void loadFile(StrView filename);

		void loadBmpMem(ByteSpan data);
		void loadBmpFile(StrView filename);

		void loadPngMem(ByteSpan data);
		void loadPngFile(StrView filename);

	void create(ColorType colorType, int width, int height);
	void create(ColorType colorType, int width, int height, int strideInBytes);
	void create(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, int dataSizeInBytes);

	SGE_INLINE	const Info&		info			() const { return _info; }
	SGE_INLINE	const Vec2i&	size			() const { return _info.size; }
	SGE_INLINE	int				width			() const { return _info.size.x; }
	SGE_INLINE	int				height			() const { return _info.size.y; }
	SGE_INLINE	int				pixelCount		() const { return _info.size.x * _info.size.y; }
	SGE_INLINE	ColorType		colorType		() const { return _info.colorType; }
	SGE_INLINE	size_t			strideInBytes	() const { return _info.strideInBytes; }
	SGE_INLINE	bool			isPreMulAlpha	() const { return _info.isPreMulAlpha; }

	void setPreMultipliedAlpha(bool enabled)	{ _info.isPreMulAlpha = enabled; };

	template<class COLOR> SGE_INLINE	Span<      COLOR>	row(int y)			{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y); }
	template<class COLOR> SGE_INLINE	Span<const COLOR>	row(int y) const	{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y); }

	template<class COLOR> SGE_INLINE	const COLOR&		pixel(int x, int y) const	{ return row<COLOR>(y)[x]; }
	template<class COLOR> SGE_INLINE		  COLOR&		pixel(int x, int y)			{ return row<COLOR>(y)[x]; }

	template<class COLOR> SGE_INLINE Span<      COLOR>		row_noCheck(int y)			{ return Span<      COLOR>(reinterpret_cast<      COLOR*>(rowBytes(y).data()), _info.size.x); }
	template<class COLOR> SGE_INLINE Span<const COLOR>		row_noCheck(int y) const	{ return Span<const COLOR>(reinterpret_cast<const COLOR*>(rowBytes(y).data()), _info.size.x); }

	template<class COLOR> void fill(const COLOR& color);

	SGE_INLINE	Span<u8>		rowBytes(int y)			{ return Span<		u8>(&_pixelData[y * _info.strideInBytes], _info.size.x * _info.pixelSizeInBytes()); }
	SGE_INLINE	Span<const u8>	rowBytes(int y) const	{ return Span<const u8>(&_pixelData[y * _info.strideInBytes], _info.size.x * _info.pixelSizeInBytes()); }

	const void* dataPtr() const { return _pixelData.data(); }

	void copyToPixelData(ByteSpan src) { _pixelData.assign(src.begin(), src.end()); }

private:
	void _create(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes);
	void _checkType(ColorType colorType) const {
		if (colorType != _info.colorType)
			throw SGE_ERROR("Invalid ColorType");
	}

	Info		_info;
	Vector<u8>	_pixelData;
};

template<class COLOR> inline
void NeHeOGL_Image::fill(const COLOR& color) {
	SGE_ASSERT(_info.colorType != ColorType::None);

	size_t stride = ColorUtil::pixelSizeInBytes(_info.colorType);
	SGE_ASSERT(stride == sizeof(color));

	u8* dst = _pixelData.begin();
	int n = pixelCount();
	for (size_t i = 0; i < n; ++i) {
		*reinterpret_cast<COLOR*>(dst) = color;
		dst += stride;
	}
	SGE_ASSERT(dst == _pixelData.end());
}

}