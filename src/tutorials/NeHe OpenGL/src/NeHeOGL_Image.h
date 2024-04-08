#pragma once

namespace sge {
	
struct NeHeOGL_ImageInfo {
	Vec2i		size { 0,0 };
	size_t		strideInBytes = 0;
	int			mipmapCount = 1;
	ColorType	colorType = ColorType::None;

	int	pixelSizeInBytes() const { return ColorUtil::pixelSizeInBytes(colorType); }
};

class NeHeOGL_Image : public NonCopyable {
public:
	using Info = NeHeOGL_ImageInfo;

	NeHeOGL_Image() = default;
	NeHeOGL_Image(NeHeOGL_Image && r) noexcept;

	void clean();
	void loadFile(StrView filename);

		void loadBmpMem(ByteSpan data);
		void loadBmpFile(StrView filename);

	void create(ColorType colorType, int width, int height);
	void create(ColorType colorType, int width, int height, size_t strideInBytes);
	void create(ColorType colorType, int width, int height, size_t strideInBytes, int mipmapCount, size_t dataSizeInBytes);

	SGE_INLINE	const Info&		info		() const { return _info; }
	SGE_INLINE	const Vec2i&	size		() const { return _info.size; }
	SGE_INLINE	int				width		() const { return _info.size.x; }
	SGE_INLINE	int				height		() const { return _info.size.y; }
	SGE_INLINE	ColorType		colorType	() const { return _info.colorType; }

	const void* dataPtr() const { return _pixelData.data(); }

	void copyToPixelData(ByteSpan src) { _pixelData.assign(src.begin(), src.end()); }

private:
	void _create(ColorType colorType, int width, int height, size_t strideInBytes, int mipmapCount, size_t dataSizeInBytes);

	Info		_info;
	Vector<u8>	_pixelData;
};

}