#pragma once

#include "MyCommon.h"

namespace sge {

namespace Utf {

	inline u32 _decodeUtf(const char8_t*& src, const char8_t* end) {
		auto v = static_cast<u8>(*src);
		u32 o = 0;
		if (v < 0x80U) {
			o = static_cast<u8>(*src++);
			return o;
		}

		if ((v & 0xE0U) == 0xC0U) {
			if (src + 2 > end) throw SGE_ERROR("_decodeUtf8 2");
			o |= (static_cast<u8>(*src) & 0x1FU) << 6;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU);		++src;
			return o;
		}

		if ((v & 0xF0U) == 0xE0U) {
			if (src + 3 > end) throw SGE_ERROR("_decodeUtf8 3");
			o |= (static_cast<u8>(*src) & 0x0FU) << 12;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU) << 6;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU);		++src;
			return o;
		}

		if ((v & 0xF8U) == 0xF0U) {
			if (src + 4 > end) throw SGE_ERROR("_decodeUtf8 4");
			o |= (static_cast<u8>(*src) & 0x07U) << 18;	++src;
			o |= (static_cast<u8>(*src) & 0x0FU) << 12;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU) << 6;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU);		++src;
			return o;
		}

		if ((v & 0xFCU) == 0xF8U) {
			if (src + 5 > end) throw SGE_ERROR("_decodeUtf8 5");
			o |= (static_cast<u8>(*src) & 0x03U) << 24;	++src;
			o |= (static_cast<u8>(*src) & 0x07U) << 18;	++src;
			o |= (static_cast<u8>(*src) & 0x0FU) << 12;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU) << 6;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU);		++src;
			return o;
		}

		if ((v & 0xFEU) == 0xFCU) {
			if (src + 6 > end) throw SGE_ERROR("_decodeUtf8 6");
			o |= (static_cast<u8>(*src) & 0x01U) << 30;	++src;
			o |= (static_cast<u8>(*src) & 0x03U) << 24;	++src;
			o |= (static_cast<u8>(*src) & 0x07U) << 18;	++src;
			o |= (static_cast<u8>(*src) & 0x0FU) << 12;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU) << 6;	++src;
			o |= (static_cast<u8>(*src) & 0x3FU);		++src;
			return o;
		}

		{
			SGE_ASSERT(FALSE);
			return static_cast<uint8_t>(*src++);
		}
	}

	inline u32 _decodeUtf(const char16_t*& src, const char16_t* end) {
		auto v = static_cast<u16>(*src);
		u32 o = 0;
		if (v >= 0xD800U && v < 0xDBFFU) {
			if (src + 2 > end) throw SGE_ERROR("_decodeUtf16 2");
			o |= (static_cast<u16>(*src) - 0xD800U) << 10;	++src;
			o |= (static_cast<u16>(*src) - 0xDC00U);		++src;
			return o;
		}

		SGE_ASSERT(src < end);
		o = static_cast<u16>(*src++);
		return o;
	}

	inline u32 _decodeUtf(const char32_t*& src, const char32_t* end) {
		SGE_ASSERT(src < end);
		return static_cast<u32>(*src++);
	}

	inline u32 _decodeUtf(const wchar_t*& src, const wchar_t* end) {
		using C = WCharUtil::Char;
		const auto*& s  = reinterpret_cast<const C*&>(src);
		const auto*  e  = reinterpret_cast<const C* >(end);
		return _decodeUtf(s, e);
	}

	template<typename OUT_STR>
	inline void _appendChar8(OUT_STR& dst, u32 v) {
		using C = typename OUT_STR::value_type;
		if (v < 0x80U) { // 1 byte 7 bit
			dst += static_cast<C>(v);
			return;
		}
		if (v < 0x800U) { // 2 bytes 5+6=11 bit
			C t[] = {
				static_cast<C>(((v >> 6) & 0x1FU) | 0xC0U),
				static_cast<C>( (v		 & 0x3FU) | 0x80U),
				static_cast<C>(0)
			};
			dst.append(t);
			return;
		}
		if (v < 0x10000U) { // 3 byte 4+6+6=16 bit
			C t[] = {
				static_cast<C>(((v >> 12) & 0x0FU) | 0xE0U),
				static_cast<C>(((v >> 6 ) & 0x3FU) | 0x80U),
				static_cast<C>( (v        & 0x3FU) | 0x80U),
				static_cast<C>(0)
			};
			dst.append(t);
			return;
		}
		if (v < 0x200000U) { // 4 byte 3+6+6+6=21 bit
			C t[] = {
				static_cast<C>(((v >> 18) & 0x07U) | 0xF0U),
				static_cast<C>(((v >> 12) & 0x3FU) | 0x80U),
				static_cast<C>(((v >> 6 ) & 0x3FU) | 0x80U),
				static_cast<C>( (v        & 0x3FU) | 0x80U),
				static_cast<C>(0)
			};
			dst.append(t);
			return;
		}

		if (v < 0x4000000U) { // 5 byte 2+6+6+6+6=26 bit
			C t[] = {
				static_cast<C>(((v >> 24) & 0x03U) | 0x1FU),
				static_cast<C>(((v >> 18) & 0x3FU) | 0x80U),
				static_cast<C>(((v >> 12) & 0x3FU) | 0x80U),
				static_cast<C>(((v >> 6 ) & 0x3FU) | 0x80U),
				static_cast<C>( (v        & 0x3FU) | 0x80U),
				static_cast<C>(0)
			};
			dst.append(t);
			return;
		}

		if (v < 0x80000000U) { // 6 byte 1+6+6+6+6+6=31 bit
			C t[] = {
				static_cast<C>(((v >> 30) & 0x01U) | 0x3FU),
				static_cast<C>(((v >> 24) & 0x3FU) | 0x80U),
				static_cast<C>(((v >> 18) & 0x3FU) | 0x80U),
				static_cast<C>(((v >> 12) & 0x3FU) | 0x80U),
				static_cast<C>(((v >> 6 ) & 0x3FU) | 0x80U),
				static_cast<C>( (v        & 0x3FU) | 0x80U),
				static_cast<C>(0)
			};
			dst.append(t);
			return;
		}

		{
			SGE_ASSERT(false);
			dst += static_cast<C>(v);
		}
	}

	template<typename OUT_STR>
	inline void _appendChar16(OUT_STR& dst, u32 v) {
		using C = typename OUT_STR::value_type;
		if ((v >= 0 && v < 0xD800U) || (v >= 0xE000U && v < 0x10000U)) { // 1 char16
			dst += static_cast<C>(v);
		} else {
			C t[] = {
				static_cast<C>((((v >> 10) & 0x3FFU) + 0xD800U)), // shift right by 10    + 0xD800 (0xD800–0xDBFF)
				static_cast<C>( ((v        & 0x3FFU) + 0xDC00U)), // take the low 10 bits + 0xDC00 (0xDC00–0xDFFF)
				static_cast<C>(0)
			};
			dst.append(t);
		}
	}

	template<typename OUT_STR>
	inline void _appendChar32(OUT_STR& dst, u32 v) {
		using C = typename OUT_STR::value_type;
		dst += static_cast<C>(v);
	}

//	template<size_t N> inline void _appendChar(StringA_ <N>& dst, u32 v) { _appendChar8 (dst, v); }
	template<size_t N> inline void _appendChar(String8_ <N>& dst, u32 v) { _appendChar8 (dst, v); }
	template<size_t N> inline void _appendChar(String16_<N>& dst, u32 v) { _appendChar16(dst, v); }
	template<size_t N> inline void _appendChar(String32_<N>& dst, u32 v) { _appendChar32(dst, v); }
	template<size_t N> inline void _appendChar(StringW_ <N>& dst, u32 v) {
		using C = WCharUtil::Char;

		if (sizeof(C) == sizeof(char16_t))
			_appendChar16(dst, v);
		else
			_appendChar32(dst, v);
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
