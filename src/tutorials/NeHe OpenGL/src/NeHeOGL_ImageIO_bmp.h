#pragma once

#include "NeHeOGL_Image.h"

typedef unsigned char stbi_uc;

namespace sge {

struct NeHeOGL_ImageIO_bmp {
	struct Reader {
		~Reader();

		static int	s_onRead(void* user, char* data, int size);
		static void s_onSkip(void* user, int byteCount);
		static int	s_onEOF(void* user);

		void load(NeHeOGL_Image& img, ByteSpan data, ColorType expectType = ColorType::None);

		int onRead(char* dst, int size);
		void onSkip(int byteCount);
		int onEOF();

		stbi_uc*	_uc = nullptr;
		ByteSpan	_data;
		const u8*	_readPtr = nullptr;
	};
};

}