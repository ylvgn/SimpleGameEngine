#pragma once

namespace sge {
#if 0
#pragma mark ========= ImageInfo ============
#endif
struct ImageInfo {
public:
	Vec2i		size {0,0};
	int			strideInBytes	= 0;
	int			mipmapCount		= 1;
	ColorType	colorType		= ColorType::None;

	SGE_INLINE int	width()				const { return size.x; }
	SGE_INLINE int	height()			const { return size.y; }
	SGE_INLINE int	pixelSizeInBytes()	const { return ColorUtil::pixelSizeInBytes(colorType); }
};


#if 0
#pragma mark ========= Image ============
#endif
//! Image for any color type
class Image : public NonCopyable {
public:
	using Info = ImageInfo;

	Image() = default;
	Image(Image && r);

	void operator=(Image&& r);

					void		clear();

					void		loadFile(StrView filename, ColorType expectType = ColorType::None);

					void		loadPngFile(StrView filename, ColorType expectType = ColorType::None);
					void		loadPngMem(ByteSpan data, ColorType expectType = ColorType::None);

					void		loadDdsFile(StrView filename, ColorType expectType = ColorType::None);
					void		loadDdsMem(ByteSpan data, ColorType expectType = ColorType::None);

					void		loadBmpFile(StrView filename, ColorType expectType = ColorType::None);
					void		loadBmpMem(ByteSpan data, ColorType expectType = ColorType::None);

					void		create		(ColorType colorType, int width, int height);
					void		create		(ColorType colorType, int width, int height, int strideInBytes);
					void		create		(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes);

	SGE_INLINE	const Info&		info			() const { return _info; }
	SGE_INLINE	const Vec2i&	size			() const { return _info.size; }
	SGE_INLINE	int				strideInBytes	() const { return _info.strideInBytes; }
	SGE_INLINE	int				pixelSizeInBytes() const { return _info.pixelSizeInBytes(); }
	SGE_INLINE	int				width			() const { return _info.width(); }
	SGE_INLINE	int				height			() const { return _info.height(); }
	SGE_INLINE	ColorType		colorType		() const { return _info.colorType; }

	template<class COLOR> SGE_INLINE	Span<      COLOR>	row(int y)			{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y); }
	template<class COLOR> SGE_INLINE	Span<const COLOR>	row(int y) const	{ _checkType(COLOR::kColorType); return row_noCheck<COLOR>(y); }

	template<class COLOR> SGE_INLINE	const COLOR&		pixel(int x, int y) const	{ return row<COLOR>(y)[x]; }
	template<class COLOR> SGE_INLINE		  COLOR&		pixel(int x, int y)			{ return row<COLOR>(y)[x]; }

	template<class COLOR> SGE_INLINE Span<      COLOR>		row_noCheck(int y)			{ return Span<      COLOR>(reinterpret_cast<      COLOR*>(rowBytes(y).data()), _info.size.x); }
	template<class COLOR> SGE_INLINE Span<const COLOR>		row_noCheck(int y) const	{ return Span<const COLOR>(reinterpret_cast<const COLOR*>(rowBytes(y).data()), _info.size.x); }

	template<class COLOR> void fill(const COLOR& color);

	SGE_INLINE	Span<u8>		rowBytes(int y)			{ return Span<      u8>(&_pixelData[y * _info.strideInBytes], _info.size.x * _info.pixelSizeInBytes()); }
	SGE_INLINE	Span<const u8>	rowBytes(int y) const	{ return Span<const u8>(&_pixelData[y * _info.strideInBytes], _info.size.x * _info.pixelSizeInBytes()); }

	void copyToPixelData(ByteSpan src) { _pixelData.assign(src.begin(), src.end()); }
	void copy(const Image& src);

	ByteSpan pixelData() const { return _pixelData.span(); }

private:
	void _create(ColorType colorType, int width, int height, int strideInBytes, int mipmapCount, size_t dataSizeInBytes);

	void _checkType(ColorType colorType) const {
		if (colorType != _info.colorType) throw SGE_ERROR("Invalid ColorType");
	}

	Info _info;
	Vector<u8>	_pixelData;
}; // Image

template<class COLOR> inline
void Image::fill(const COLOR& color) {
	_checkType(color.kColorType);
	for (int y = 0; y < width(); ++y) {
		auto span = row_noCheck<COLOR>(y);
		for (int x = 0; x < height(); ++x) {
			span[x].set(color);
		}
	}
}

} // namespace sge