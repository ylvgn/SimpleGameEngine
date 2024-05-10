#pragma once

#include "NeHeOGL_Image.h"

typedef unsigned char stbi_uc;

namespace sge {

struct NeHeOGL_ImageIO_bmp {
	struct Reader {
		~Reader();

		void load(NeHeOGL_Image& img, ByteSpan data, ColorType expectType = ColorType::None);

		static int	s_onRead(void* user, char* data, int size);
		static void s_onSkip(void* user, int size);
		static int	s_onEOF(void* user);

		int			onRead(char* dst, int size);
		void		onSkip(int size);
		int			onEOF();

		stbi_uc*	_uc = nullptr;
		ByteSpan	_data;
		const u8*	_readPtr = nullptr;
	};
};

}