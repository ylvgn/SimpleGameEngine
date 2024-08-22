#pragma once

#include "Image.h"

namespace sge {

struct BITMAPCOREHEADER;
struct OS22XBITMAPHEADER;
struct BITMAPINFOHEADER;
//struct BITMAPV2HEADER; // Undocumented
struct BITMAPV3HEADER;
struct BITMAPV4HEADER;
struct BITMAPV5HEADER;

struct ImageIO_bmp {

	class Reader {
	public:
		void load(Image& img, ByteSpan data, ColorType expectType = ColorType::None);

	private:
		struct BMPInfo {
			u32 offset		= 0;
			int width		= 0;
			int height		= 0;
			u32 cbSize		= 0;
			u32 bpp			= 0;
			u32 compress	= 0;

			u32 maskR		= 0;
			u32 maskG		= 0;
			u32 maskB		= 0;
			u32 maskA		= 0;

			ColorType colorType = ColorType::None;
		};

		void _readHeader(BinDeserializer& de, BITMAPCOREHEADER& hdr);
		void _readHeader(BinDeserializer& de, OS22XBITMAPHEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPINFOHEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPV3HEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPV4HEADER& hdr);
		void _readHeader(BinDeserializer& de, BITMAPV5HEADER& hdr);

		void _throwIfInvalidNumPlanes(int numPlanes) {
			if (numPlanes != 1)
				throw SGE_ERROR("bmp error: The number of planes for the target device, it must be 1");
		}

		void _swapBGRAToRGBA(Image& out);
		template<class COLOR> void _swapColorChannel(Image& out, int fromChannel, int toChannel) {
			SGE_ASSERT(out.colorType() != ColorType::None);
			SGE_ASSERT(ColorUtil::colorModel(out.colorType()) == ColorModel::RGB 
					|| ColorUtil::colorModel(out.colorType()) == ColorModel::RGBA);

			for (int y = 0; y < out.width(); ++y) {
				auto row = out.row<COLOR>(y);
				for (int x = 0; x < out.height(); ++x) {
					swap(row[x].data[fromChannel], row[x].data[toChannel]);
				}
			}
		}

		void _readPixelData(Image& out, const BMPInfo& info, ByteSpan srcPixelData);

		template<class DST_COLOR, class SRC_COLOR>
		void _readPixelDataFromRGBorRGBA(	Image& out,
											const BMPInfo& info,
											ByteSpan srcPixelData,
											typename DST_COLOR::ElementType filler = 0) {

			SGE_ASSERT(SRC_COLOR::kColorType == info.colorType);
			SGE_ASSERT(SRC_COLOR::kElementCount >= 3);

			using SRC_ElementType = SRC_COLOR::ElementType;
			using DST_ElementType = DST_COLOR::ElementType;

			auto& width  = info.width;
			auto& height = info.height;
				
			out.create(DST_COLOR::kColorType, width, height);

			bool out_has_alpha = ColorUtil::hasAlpha(out.colorType());

			auto span = spanCast<const SRC_ElementType>(srcPixelData);
			auto* p = span.begin();
			for (int y = 0; y < width; ++y) {
				auto rowSpan = out.row<DST_COLOR>(y);
				for (int x = 0; x < height; ++x) {
					rowSpan[x].data[0] = static_cast<DST_ElementType>(*(p+2));
					rowSpan[x].data[1] = static_cast<DST_ElementType>(*(p+1));
					rowSpan[x].data[2] = static_cast<DST_ElementType>(*(p+0));
					p += 3;
					if (out_has_alpha) rowSpan[x].data[3] = filler;
				}
			}
			SGE_ASSERT(p == span.end());
		}
	};
};

} //namespace
