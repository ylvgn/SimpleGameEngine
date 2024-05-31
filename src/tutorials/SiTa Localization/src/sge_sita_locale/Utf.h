#pragma once

#include "MyCommon.h"

namespace sge {

namespace Utf {

	void decodeUtf8(StringW& o, StrView src) {
		auto* p = src.begin();
		auto* ed = src.end();

		while (p < ed) {
			auto ch = static_cast<u8>(*p); // 1 byte
			if (ch < 0x80U) {
				auto a = static_cast<wchar_t>(*p++);
				o.push_back(a);
				continue;
			}

			if ((ch & 0xE0U) == 0xC0U) { // read 2 bytes
				if (p + 2 > ed) throw SGE_ERROR("_decodeUtf8 2");
				auto a = static_cast<wchar_t>(*p++);
				auto b = static_cast<wchar_t>(*p++);
				o.push_back( ((a & 0x1FU) << 6) | (b & 0x3FU) );
				continue;
			}

			if ((ch & 0xF0U) == 0xE0U) { // read 3 bytes
				if (p + 3 > ed) throw SGE_ERROR("_decodeUtf8 3");
				auto a = static_cast<wchar_t>(*p++);
				auto b = static_cast<wchar_t>(*p++);
				auto c = static_cast<wchar_t>(*p++);
				o.push_back( ((a & 0x0FU) << 12) | ((b & 0x3FU) << 6) | (c & 0x3FU) );
				continue;
			}


			if ((ch & 0xF8U) == 0xF0U) { // read 4 bytes
				if (p + 4 > ed) throw SGE_ERROR("_decodeUtf8 4");
				auto a = static_cast<wchar_t>(*p++);
				auto b = static_cast<wchar_t>(*p++);
				auto c = static_cast<wchar_t>(*p++);
				auto d = static_cast<wchar_t>(*p++);
				o.push_back( ((a & 0x07U) << 18) | ((b & 0x3FU) << 12) | ((c & 0x3FU) << 6) | (d & 0x3FU) );
				continue;
			}

// predict 5 bytes, 6 bytes
			if ((ch & 0xFCU) == 0xF8U) { // read 5 bytes
				if (p + 5 > ed) throw SGE_ERROR("_decodeUtf8 5");
				auto a = static_cast<wchar_t>(*p++);
				auto b = static_cast<wchar_t>(*p++);
				auto c = static_cast<wchar_t>(*p++);
				auto d = static_cast<wchar_t>(*p++);
				auto e = static_cast<wchar_t>(*p++);
				o.push_back( ((a & 0x03U) << 24) | ((b & 0x3FU) << 18) | ((c & 0x3FU) << 12) | ((d & 0x3FU) << 6) | (e & 0x3FU) );
				continue;
			}

			if ((ch & 0xFEU) == 0xFCU) { // read 6 bytes
				if (p + 6 > ed) throw SGE_ERROR("_decodeUtf8 6");
				auto a = static_cast<wchar_t>(*p++);
				auto b = static_cast<wchar_t>(*p++);
				auto c = static_cast<wchar_t>(*p++);
				auto d = static_cast<wchar_t>(*p++);
				auto e = static_cast<wchar_t>(*p++);
				auto f = static_cast<wchar_t>(*p++);
				o.push_back(((a & 0x01U) << 30) | ((b & 0x3FU) << 24) | ((c & 0x3FU) << 18) | ((d & 0x3FU) << 12) | ((e & 0x3FU) << 6) | (f & 0x3FU) );
				continue;
			}

			throw SGE_ERROR("_decodeUtf8 7");
		}

		SGE_ASSERT(p == ed);
	}

	void encodeUtf8(StringA& o, StringW src) {
		auto* p  = src.begin();
		auto* ed = src.end();

		for (; p != ed; p++) {
			auto c = static_cast<u32>(*p);
			if (c < 0x80U) { // 1 byte 7 bit
				o.push_back(static_cast<char>(c));
				continue;
			}
			if (c < 0x800U) { // 2 bytes 5+6=11 bit
				char ch[] = {
					static_cast<char>( ((c >> 6)	& 0x1FU) | 0xC0U),
					static_cast<char>( ( c			& 0x3FU) | 0x80U),
					static_cast<char>(0)
				};
				o.append(ch);
				continue;
			}
			if (c < 0x10000U) { // 3 byte 4+6+6=16 bit
				char ch[] = {
					static_cast<char>( ((c >> 12)	& 0x0FU) | 0xE0U),
					static_cast<char>( ((c >> 6 )	& 0x3FU) | 0x80U),
					static_cast<char>( ( c			& 0x3FU) | 0x80U),
					static_cast<char>(0)
				};
				o.append(ch);
				continue;
			}
			if (c < 0x200000U) { // 4 byte 3+6+6+6=21 bit
				char ch[] = {
					static_cast<char>( ((c >> 18)	& 0x07U) | 0xF0U),
					static_cast<char>( ((c >> 12)	& 0x3FU) | 0x80U),
					static_cast<char>( ((c >> 6 )	& 0x3FU) | 0x80U),
					static_cast<char>( ( c			& 0x3FU) | 0x80U),
					static_cast<char>(0)
				};
				o.append(ch);
				continue;
			}
// predict 5 bytes, 6 bytes
			if (c < 0x4000000U) { // 5 byte 2+6+6+6+6=26 bit
				char ch[] = {
					static_cast<char>( ((c >> 24)	& 0x03U) | 0x1FU),
					static_cast<char>( ((c >> 18)	& 0x3FU) | 0x80U),
					static_cast<char>( ((c >> 12)	& 0x3FU) | 0x80U),
					static_cast<char>( ((c >> 6 )	& 0x3FU) | 0x80U),
					static_cast<char>( ( c			& 0x3FU) | 0x80U),
					static_cast<char>(0)
				};
				o.append(ch);
				continue;
			}

			if (c < 0x80000000U) { // 6 byte 1+6+6+6+6+6=31 bit
				char ch[] = {
					static_cast<char>( ((c >> 30)	& 0x01U) | 0x3FU),
					static_cast<char>( ((c >> 24)	& 0x3FU) | 0x80U),
					static_cast<char>( ((c >> 18)	& 0x3FU) | 0x80U),
					static_cast<char>( ((c >> 12)	& 0x3FU) | 0x80U),
					static_cast<char>( ((c >> 6 )	& 0x3FU) | 0x80U),
					static_cast<char>( ( c			& 0x3FU) | 0x80U),
					static_cast<char>(0)
				};
				o.append(ch);
				continue;
			}
		}
		SGE_ASSERT(p == ed);
	}
}

}
