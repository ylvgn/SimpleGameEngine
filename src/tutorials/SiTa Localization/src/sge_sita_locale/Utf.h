#pragma once

#include "MyCommon.h"

namespace sge {

namespace Utf {

	inline u32 _decodeUtf(const char8_t*& p, const char8_t* ed) {
		auto ch = static_cast<u8>(*p);

		if (ch < 0x80U) {
			return static_cast<u8>(*p++);
		}

		if ((ch & 0xE0U) == 0xC0U) {
			if (p + 2 > ed) throw SGE_ERROR("_decodeUtf8 2");
			u32 a = static_cast<u8>(*p++);
			u32 b = static_cast<u8>(*p++);
			return ((a & 0x1FU) << 6) | (b & 0x3FU);
		}

		if ((ch & 0xF0U) == 0xE0U) {
			if (p + 3 > ed) throw SGE_ERROR("_decodeUtf8 3");
			u32 a = static_cast<u8>(*p++);
			u32 b = static_cast<u8>(*p++);
			u32 c = static_cast<u8>(*p++);
			return ((a & 0x0FU) << 12) | ((b & 0x3FU) << 6) | (c & 0x3FU);
		}

		if ((ch & 0xF8U) == 0xF0U) {
			if (p + 4 > ed) throw SGE_ERROR("_decodeUtf8 4");
			u32 a = static_cast<u8>(*p++);
			u32 b = static_cast<u8>(*p++);
			u32 c = static_cast<u8>(*p++);
			u32 d = static_cast<u8>(*p++);
			return ((a & 0x07U) << 18) | ((b & 0x3FU) << 12) | ((c & 0x3FU) << 6) | (d & 0x3FU);
		}

		if ((ch & 0xFCU) == 0xF8U) {
			if (p + 5 > ed) throw SGE_ERROR("_decodeUtf8 5");
			u32 a = static_cast<u8>(*p++);
			u32 b = static_cast<u8>(*p++);
			u32 c = static_cast<u8>(*p++);
			u32 d = static_cast<u8>(*p++);
			u32 e = static_cast<u8>(*p++);
			return ((a & 0x03U) << 24) | ((b & 0x3FU) << 18) | ((c & 0x3FU) << 12) | ((d & 0x3FU) << 6) | (e & 0x3FU);
		}

		if ((ch & 0xFEU) == 0xFCU) { // read 6 bytes
			if (p + 6 > ed) throw SGE_ERROR("_decodeUtf8 6");
			u32 a = static_cast<u8>(*p++);
			u32 b = static_cast<u8>(*p++);
			u32 c = static_cast<u8>(*p++);
			u32 d = static_cast<u8>(*p++);
			u32 e = static_cast<u8>(*p++);
			u32 f = static_cast<u8>(*p++);
			return ((a & 0x01U) << 30) | ((b & 0x3FU) << 24) | ((c & 0x3FU) << 18) | ((d & 0x3FU) << 12) | ((e & 0x3FU) << 6) | (f & 0x3FU);
		}

		{
			SGE_ASSERT(FALSE);
			return static_cast<uint8_t>(*p++);
		}
	}

	inline u32 _decodeUtf(const char16_t*& p, const char16_t* ed) {
		auto ch = static_cast<u16>(*p);

		if (ch >= 0xD800U && ch < 0xDBFFU) {
			if (p + 2 > ed) throw SGE_ERROR("_decodeUtf16 2");
			u32 a = static_cast<u16>(*p++); // high 10-bit
			u32 b = static_cast<u16>(*p++); // low  6-bit
			return ((a - 0xD800U) << 10) | (b - 0xDC00U);
		}

		SGE_ASSERT(p < ed);
		return static_cast<u16>(*p++);
	}

	inline u32 _decodeUtf(const char32_t*& p, const char32_t* ed) {
		SGE_ASSERT(p < ed);
		return static_cast<u32>(*p++);
	}

	inline u32 _decodeUtf(const wchar_t*& p, const wchar_t* ed) {
		using C = WCharUtil::Char;
		const auto*& s  = reinterpret_cast<const C*&>(p);
		const auto*  e  = reinterpret_cast<const C*>(ed);
		return _decodeUtf(s, e);
	}

	template<typename OUT_STR>
	inline void _appendChar8(OUT_STR& dst, u32 c) {
		if (c < 0x80U) { // 1 byte 7 bit
			dst += static_cast<char>(c);
			return;
		}
		if (c < 0x800U) { // 2 bytes 5+6=11 bit
			char ch[] = {
				static_cast<char>(((c >> 6) & 0x1FU) | 0xC0U),
				static_cast<char>( (c & 0x3FU)       | 0x80U),
				0,
			};
			dst.append(ch);
			return;
		}
		if (c < 0x10000U) { // 3 byte 4+6+6=16 bit
			char ch[] = {
				static_cast<char>(((c >> 12) & 0x0FU) | 0xE0U),
				static_cast<char>(((c >> 6)  & 0x3FU) | 0x80U),
				static_cast<char>( (c        & 0x3FU) | 0x80U),
				0,
			};
			dst.append(ch);
			return;
		}
		if (c < 0x200000U) { // 4 byte 3+6+6+6=21 bit
			char ch[] = {
				static_cast<char>(((c >> 18) & 0x07U) | 0xF0U),
				static_cast<char>(((c >> 12) & 0x3FU) | 0x80U),
				static_cast<char>(((c >> 6)  & 0x3FU) | 0x80U),
				static_cast<char>( (c        & 0x3FU) | 0x80U),
				0
			};
			dst.append(ch);
			return;
		}

		if (c < 0x4000000U) { // 5 byte 2+6+6+6+6=26 bit
			char ch[] = {
				static_cast<char>(((c >> 24) & 0x03U) | 0x1FU),
				static_cast<char>(((c >> 18) & 0x3FU) | 0x80U),
				static_cast<char>(((c >> 12) & 0x3FU) | 0x80U),
				static_cast<char>(((c >> 6 ) & 0x3FU) | 0x80U),
				static_cast<char>( (c        & 0x3FU) | 0x80U),
				0
			};
			dst.append(ch);
			return;
		}

		if (c < 0x80000000U) { // 6 byte 1+6+6+6+6+6=31 bit
			char ch[] = {
				static_cast<char>(((c >> 30) & 0x01U) | 0x3FU),
				static_cast<char>(((c >> 24) & 0x3FU) | 0x80U),
				static_cast<char>(((c >> 18) & 0x3FU) | 0x80U),
				static_cast<char>(((c >> 12) & 0x3FU) | 0x80U),
				static_cast<char>(((c >> 6)  & 0x3FU) | 0x80U),
				static_cast<char>( (c        & 0x3FU) | 0x80U),
				0
			};
			dst.append(ch);
			return;
		}

		{
			SGE_ASSERT(false);
			dst += static_cast<char>(c);
		}
	}

	template<typename OUT_STR>
	inline void _appendChar16(OUT_STR& dst, u32 c) {
		using C = typename OUT_STR::value_type;
		if ((c >= 0 && c < 0xD800U) || (c >= 0xE000U && c < 0x10000U)) { // 1 char16
			dst += static_cast<C>(c);
		} else {
			C ch[] = {
				static_cast<C>(((c >> 10) & 0x3FFU + 0xD800U)), // shift right by 10 + 0xD800    (0xD800–0xDBFF)
				static_cast<C>(( c        & 0x3FFU + 0xDC00U)), // take the low 10 bits + 0xDC00 (0xDC00–0xDFFF)
				static_cast<C>(0)
			};
			dst.append(ch);
		}
	}

	template<typename OUT_STR>
	inline void _appendChar32(OUT_STR& dst, u32 c) {
		using C = typename OUT_STR::value_type;
		dst += static_cast<C>(c);
	}

//	template<size_t N> inline void _appendChar(StringA_ <N>& dst, u32 c) { _appendChar8 (dst, c); }
	template<size_t N> inline void _appendChar(String8_ <N>& dst, u32 c) { _appendChar8 (dst, c); }
	template<size_t N> inline void _appendChar(String16_<N>& dst, u32 c) { _appendChar16(dst, c); }
	template<size_t N> inline void _appendChar(String32_<N>& dst, u32 c) { _appendChar32(dst, c); }
	template<size_t N> inline void _appendChar(StringW_ <N>& dst, u32 c) {
		using C = WCharUtil::Char;

		if (sizeof(C) == sizeof(char16_t))
			_appendChar16(dst, c);
		else
			_appendChar32(dst, c);
	}

	template<typename DST, typename SRC>
	inline void _append(DST& dst, SRC src) {
		auto* p = src.begin();
		auto* e = src.end();
		while (p < e) {
			auto c = _decodeUtf(p, e);
			_appendChar(dst, c);
		}
	}

	template<size_t N> inline void append(StringA_<N>& dst, StrViewW src) { _append(dst, src); }
	template<size_t N> inline void append(StringW_<N>& dst, StrViewA src) { _append(dst, src); }

	template<typename DST, typename SRC>
	inline void convert(DST& dst, SRC src) {
		dst.clear();
		append(dst, src);
	}
}

}
