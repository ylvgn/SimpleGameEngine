#pragma once

#include "../base/Error.h"

namespace sge {

struct UtfUtil {
	UtfUtil() = delete;

	template<class DST, class SRC> static void convert(DST& dst, const SRC& src);

	template<class SRC>	static String  toString	(SRC& src) { String  o; convert(o, src); return o; }
	template<class SRC>	static StringW toStringW(SRC& src) { StringW o; convert(o, src); return o; }

private:
	template<class DST, class SRC> static void _append_impl(DST& dst, SRC view);

	template<size_t N>	static void _append(String8_<N> & dst, StrView8  view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(String8_<N> & dst, StrView16 view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(String8_<N> & dst, StrView32 view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(String8_<N> & dst, StrViewW  view) { _append_impl(dst, view); }

	template<size_t N>  static void _append(String16_<N>& dst, StrView8  view) { _append_impl(dst, view); }
	template<size_t N>  static void _append(String16_<N>& dst, StrView16 view) { _append_impl(dst, view); }
	template<size_t N>  static void _append(String16_<N>& dst, StrView32 view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(String16_<N>& dst, StrViewW  view) { _append_impl(dst, view); }

	template<size_t N>  static void _append(String32_<N>& dst, StrView8  view) { _append_impl(dst, view); }
	template<size_t N>  static void _append(String32_<N>& dst, StrView16 view) { _append_impl(dst, view); }
	template<size_t N>  static void _append(String32_<N>& dst, StrView32 view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(String32_<N>& dst, StrViewW  view) { _append_impl(dst, view); }

	template<size_t N>	static void _append(StringW_<N> & dst, StrView8  view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(StringW_<N> & dst, StrView16 view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(StringW_<N> & dst, StrView32 view) { _append_impl(dst, view); }
	template<size_t N>	static void _append(StringW_<N> & dst, StrViewW  view) { _append_impl(dst, view); }

	static uint32_t _decodeUtf(const char*&     src, const char*     end); // char8_t: require c++20
	static uint32_t _decodeUtf(const char16_t*& src, const char16_t* end);
	static uint32_t _decodeUtf(const char32_t*& src, const char32_t* end);
	static uint32_t _decodeUtf(const wchar_t*&  src, const wchar_t*  end);

	template<size_t N>	static void _encodeUtf(String8_<N> & dst, uint32_t v);
	template<size_t N>	static void _encodeUtf(String16_<N>& dst, uint32_t v);
	template<size_t N>	static void _encodeUtf(String32_<N>& dst, uint32_t v);
	template<size_t N>	static void _encodeUtf(StringW_<N> & dst, uint32_t v) {
		using C   = typename TypeTraits::typeBySize<sizeof(wchar_t)>::Char;
		using STR = typename StringT<C, N, true>;
		_encodeUtf(reinterpret_cast<STR&>(dst), v);
	}
}; // UtfUtil

template<size_t N> SGE_INLINE
void UtfUtil::_encodeUtf(String8_<N>& dst, uint32_t v) {
	using C = char;

	if ( v <       0x80U ) {
		dst += C(v);
		return;
	}

	if ( v <      0x800U ) {
		C t[] = {
			C(((v >> 6) & 0x1FU) | 0xC0U),
			C(( v		& 0x3FU) | 0x80U),
			C(0)
		};
		dst.append(t);
		return;
	}

	if ( v <     0x10000U ) {
		C t[] = {
			C(((v >> 12) & 0x0FU) | 0xE0U),
			C(((v >> 6)  & 0x3FU) | 0x80U),
			C(( v		 & 0x3FU) | 0x80U),
			C(0)
		};
		dst.append(t);
		return;
	}

	if ( v <   0x200000U ) {
		C t[] = {
			C(((v >> 18) & 0x07U) | 0xF0U),
			C(((v >> 12) & 0x3FU) | 0x80U),
			C(((v >> 6)  & 0x3FU) | 0x80U),
			C(( v		 & 0x3FU) | 0x80U),
			C(0)
		};
		dst.append(t);
		return;
	}

// The patterns below are not part of UTF-8, but were part of the first specification.
	if ( v <  0x4000000U ) {
		C t[] = {
			C(((v >> 24) & 0x03U) | 0xF8U),
			C(((v >> 18) & 0x3FU) | 0x80U),
			C(((v >> 12) & 0x3FU) | 0x80U),
			C(((v >> 6)  & 0x3FU) | 0x80U),
			C(( v		 & 0x3FU) | 0x80U),
			C(0)
		};
		dst.append(t);
		return;
	}

	if ( v < 0x80000000U ) {
		C t[] = {
			C(((v >> 30) & 0x01U) | 0xFCU),
			C(((v >> 24) & 0x3FU) | 0x80U),
			C(((v >> 18) & 0x3FU) | 0x80U),
			C(((v >> 12) & 0x3FU) | 0x80U),
			C(((v >> 6 ) & 0x3FU) | 0x80U),
			C(( v		 & 0x3FU) | 0x80U),
			C(0)
		};
		dst.append(t);
		return;
	}

	{
		SGE_ASSERT(false);
		dst += static_cast<C>(v);
	}
}

template<size_t N> SGE_INLINE
void UtfUtil::_encodeUtf(String16_<N>& dst, uint32_t v) {
	using C = char16_t;

	if ((v < 0xD800U) || (v >= 0xE000U && v < 0x10000U)) {
		dst += C(v);
		return;
	}

	if (v < 0x110000U) {
		C t[] = {
			C((((v >> 10) & 0x3FFU) + 0xD800U)),
			C(((v & 0x3FFU) + 0xDC00U)),
			C(0)
		};
		dst.append(t);
		return;
	}

	{
		SGE_ASSERT(false);
		dst += static_cast<C>(v);
		return;
	}
}

template<size_t N> SGE_INLINE
void UtfUtil::_encodeUtf(String32_<N>& dst, uint32_t v) {
	using C = char32_t;
	dst += static_cast<C>(v);
}

template<class DST, class SRC> SGE_INLINE
void UtfUtil::convert(DST& dst, const SRC& src) {
	dst.clear();
	_append(dst, src);
}

template<class DST, class SRC> SGE_INLINE
void UtfUtil::_append_impl(DST& dst, SRC src) {
	const auto* s = src.begin();
	const auto* e = src.end();

	while (s < e) {
		auto v = _decodeUtf(s, e);
		_encodeUtf(dst, v);
	}
}

SGE_INLINE
uint32_t UtfUtil::_decodeUtf(const char* & src, const char* end) {
	auto v = static_cast<uint8_t>(*src);
	uint32_t o = 0;

	if (v < 0x80U) {
		o = uint8_t(*src); ++src;
		return o;
	}

	if ((v & 0xE0U) == 0xC0U) {
		if (src + 2 > end) throw SGE_ERROR("");
		o += (uint8_t(*src) & 0x1FU) << 6; ++src;
		o += (uint8_t(*src) & 0x3FU);      ++src;
		return o;
	}

	if ((v & 0xF0U) == 0xE0U) {
		if (src + 3 > end) throw SGE_ERROR("");
		o += (uint8_t(*src) & 0x0FU) << 12; ++src;
		o += (uint8_t(*src) & 0x3FU) << 6;  ++src;
		o += (uint8_t(*src) & 0x3FU);       ++src;
		return o;
	}

	if ((v & 0xF8U) == 0xF0U) {
		if (src + 4 > end) throw SGE_ERROR("");
		o += (uint8_t(*src) & 0x07U) << 18; ++src;
		o += (uint8_t(*src) & 0x3FU) << 12; ++src;
		o += (uint8_t(*src) & 0x3FU) << 6;  ++src;
		o += (uint8_t(*src) & 0x3FU);       ++src;
		return o;
	}

	if ((v & 0xFCU) == 0xF8U) {
		if (src + 5 > end) throw SGE_ERROR("");
		o += (uint8_t(*src) & 0x03U) << 24; ++src;
		o += (uint8_t(*src) & 0x3FU) << 18; ++src;
		o += (uint8_t(*src) & 0x3FU) << 12; ++src;
		o += (uint8_t(*src) & 0x3FU) << 6;  ++src;
		o += (uint8_t(*src) & 0x3FU);       ++src;
	}

	if ((v & 0xFEU) == 0xFCU) {
		if (src + 6 > end) throw SGE_ERROR("");
		o += (uint8_t(*src) & 0x01U) << 30; ++src;
		o += (uint8_t(*src) & 0x3FU) << 24; ++src;
		o += (uint8_t(*src) & 0x3FU) << 18; ++src;
		o += (uint8_t(*src) & 0x3FU) << 12; ++src;
		o += (uint8_t(*src) & 0x3FU) << 6;  ++src;
		o += (uint8_t(*src) & 0x3FU);       ++src;
		return o;
	}

	{
		SGE_ASSERT(false);
		o = static_cast<uint8_t>(*src); ++src;
		return o;
	}
}

SGE_INLINE
uint32_t UtfUtil::_decodeUtf(const char16_t*& src, const char16_t* end) {
	auto v = static_cast<uint16_t>(*src);

	if (v >= 0xD800U && v < 0xDBFFU) {
		if( src+2 > end ) throw SGE_ERROR("");
		uint32_t a = static_cast<uint16_t>(*src); ++src;
		uint32_t b = static_cast<uint16_t>(*src); ++src;
		return ((a - 0xD800U) << 10) | (b - 0xDC00U);
	}

	uint32_t a = static_cast<uint16_t>(*src); ++src;
	return a;
}

SGE_INLINE
uint32_t UtfUtil::_decodeUtf(const char32_t*& src, const char32_t* end) {
	return *src++;
}

SGE_INLINE
uint32_t UtfUtil::_decodeUtf(const wchar_t*& src, const wchar_t* end) {
	using C = WCharUtil::Char;
	const auto* & s = reinterpret_cast<const C* &>(src);
	const auto*   e = reinterpret_cast<const C*  >(end);
	return _decodeUtf(s, e);
}

} // namespace sge