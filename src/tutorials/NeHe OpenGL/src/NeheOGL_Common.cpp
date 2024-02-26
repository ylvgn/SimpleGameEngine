#include "NeheOGL_Common.h"
#include "externals/stb_image.h"

namespace sge {

struct ImageIO_bmp {
	struct Reader {
		~Reader() {
			if (_uc) {
				stbi_image_free(_uc);
				_uc = nullptr;
			}
		}

		static int s_onRead(void* user, char* data, int size) {
			//SGE_LOG("s_onRead size={}", size);
			auto* thisObj = static_cast<Reader*>(user);
			if (!thisObj) {
				throw SGE_ERROR("bmp error s_onRead");
			}
			return thisObj->onRead(data, size);
		}

		static void s_onSkip(void* user, int byteCount) {
			//SGE_LOG("s_onSkip byteCount={}", byteCount);
			auto* thisObj = static_cast<Reader*>(user);
			if (!thisObj) {
				throw SGE_ERROR("bmp error s_onSkip");
			}
			thisObj->onSkip(byteCount);
		}

		static int s_onEOF(void* user) {
			//SGE_LOG("s_onEOF");
			auto* thisObj = static_cast<Reader*>(user);
			if (!thisObj) {
				throw SGE_ERROR("bmp error s_onEOF");
			}
			return thisObj->onEOF();
		}

		void load(MyImage& img, ByteSpan data, ColorType expectType = ColorType::None) {
			_data = data;
			_readPtr = data.data();
			int dataSize = static_cast<int>(_data.size());

			int comp;
			int req_comp;

			int ok = stbi_info_from_memory(_data.data(), dataSize, &img.width, &img.height, &comp);
			if (!ok)
				throw SGE_ERROR("bmp error stbi_info_from_memory");

			switch (expectType) {
				case ColorType::None:	req_comp = STBI_default; break;
				case ColorType::RGBb:	req_comp = STBI_rgb; break;
				case ColorType::RGBAb:	req_comp = STBI_rgb_alpha; break;
				default:				throw SGE_ERROR("bmp error unsupported expectType = {}", expectType);
			}

#if 0
			stbi_io_callbacks clbk;
			clbk.read = s_onRead;
			clbk.skip = s_onSkip;
			clbk.eof = s_onEOF;
			_uc = stbi_load_from_callbacks(&clbk, this, &img.width, &img.height, &img.channels, req_comp);
#else
			_uc = stbi_load_from_memory(_data.data(), dataSize, &img.width, &img.height, &comp, req_comp);
#endif

			if (!_uc)
				throw SGE_ERROR("bmp error stbi_load_from_memory");

			size_t strideInBytes   = img.width * ColorUtil::pixelSizeInBytes(expectType);
			size_t dataSizeInBytes = img.height * strideInBytes;
			img.pixelData.reserve(dataSizeInBytes);
			memcpy(img.pixelData.data(), _uc, dataSizeInBytes);
		}

		int onRead(char* dst, int size) {
			int n = size;
			if (_readPtr + n > _data.end()) {
				n = static_cast<int>(_data.end() - _readPtr);
				if (!n)
					throw SGE_ERROR("bmp error onRead size = {}", size);
			}
			memcpy(dst, _readPtr, n);
			_readPtr += n;
			return n;
		}

		void onSkip(int byteCount) {
			// unknown
		}

		int onEOF() {
			return _readPtr == _data.end();
		}

		stbi_uc*	_uc = nullptr;
		ByteSpan	_data;
		const u8*	_readPtr = nullptr;
	};
};

void MyImage::loadBmpMem(ByteSpan data) {
	ImageIO_bmp::Reader r;
	r.load(*this, data, ColorType::RGBAb);
}
	
}