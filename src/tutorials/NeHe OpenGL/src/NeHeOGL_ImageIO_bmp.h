#pragma once

#include "NeHeOGL_Image.h"

// stb_image.h
typedef unsigned char	stbi_uc;
typedef unsigned short	stbi_us;

namespace sge {

struct NeHeOGL_ImageIO_bmp {
	struct Reader {
		~Reader();

		void load(NeHeOGL_Image& img, ByteSpan data, ColorType expectType = ColorType::None);

		static int s_onRead(void* io_user_data, char* dest, int len);
		int	onRead(char* dest, int len);

		static void s_onSkip(void* io_user_data, int len);
		void onSkip(int len);

		static int s_onEOF(void* io_user_data);
		int	onEOF();

		::stbi_uc*	_uc = nullptr;
		ByteSpan	_data;
		const u8*	_readPtr = nullptr;
	};
};

}