#include "StringUtil.h"
#include "UtfUtil.h"
#include <sge_core/math/Math.h>

namespace sge {

void StringUtil::binToHex(String& out, Span<u8> data) {
	out.clear();
	appendBinToHex(out, data);
}

void StringUtil::appendBinToHex(String& out, ByteSpan data) {
	const char* hex = "0123456789ABCDEF";
	size_t lineCount = (data.size() + 15) / 16;

	for (size_t line = 0; line < lineCount; line++) {
		size_t lineStart = line * 16;
		for (size_t i = 0; i < 16; i++) {
			size_t index = lineStart + i;

			if (index < data.size()) {
				u8 v = data[index];
				out.push_back(hex[v >> 4]);
				out.push_back(hex[v & 0xF]);
			}
			else {
				out.append("  ");
			}
			out.push_back(' ');
		}

		out.append("    ");

		for (size_t i = 0; i < 16; i++) {
			size_t index = lineStart + i;

			if (index < data.size()) {
				u8 v = data[index];
				if (v < 32) {
					out.push_back(' '); // non-printable character
				}
				else {
					out.push_back(v);
				}
			}
			else {
				out.push_back(' ');
			}
//			out.push_back(' ');
		}
		out.push_back('\n');
	}
}

const char* StringUtil::findChar(StrView view, StrView charList, bool ignoreCase) {
	auto* start = view.data();
	auto* end = start + view.size();
	auto* p = start;

	if (ignoreCase) {
		for ( ; p < end; p++ ) {
			for (const auto& ch : charList) {
				if (ignoreCaseCompare(*p, ch) == 0) {
					return p;
				}
			}
		}
	} else {
		for ( ; p < end; p++ ) {
			for (const auto& ch : charList) {
				if (*p == ch) {
					return p;
				}
			}
		}
	}
	return nullptr;
}

const char* StringUtil::findCharFromEnd(StrView view, StrView charList, bool ignoreCase) {
	if (view.size() <= 0) return nullptr;

	auto* start = view.data();
	auto* end = start + view.size();
	auto* p = end - 1;

	if (ignoreCase) {
		for ( ; p >= start; p-- ) {
			for(auto& ch : charList) {
				if (ignoreCaseCompare(*p, ch) == 0) {
					return p;
				}
			}
		}
	} else {
		for ( ; p >= start; p-- ) {
			for(auto& ch : charList) {
				if (*p == ch) {
					return p;
				}
			}
		}
	}
	return nullptr;
}

struct StringUtil_ParseHelper {
	StringUtil_ParseHelper() = delete;

	template<class T> SGE_INLINE
	static bool tryParseInt(StrView view, T& outValue) {
		SGE_STATIC_ASSERT(TypeTraits::isSigned<T>::value);
		String_<256> tmp = view;
		i64 v;
		auto ret = ::sscanf(tmp.c_str(), "%lld", &v);
		if (ret != 1) return false;
		if (v < std::numeric_limits<T>::min()) return false;
		if (v > std::numeric_limits<T>::max()) return false;
		outValue = static_cast<T>(v);
		return true;
	}

	template<class T> SGE_INLINE
	static bool tryParseUInt(StrView view, T& outValue) {
		SGE_STATIC_ASSERT(std::is_unsigned<T>::value);
		String_<256> tmp = view;
		u64 v;
		auto ret = ::sscanf(tmp.c_str(), "%llu", &v);
		if (ret != 1) return false;
		if (v < std::numeric_limits<T>::min()) return false;
		if (v > std::numeric_limits<T>::max()) return false;
		outValue = static_cast<T>(v);
		return true;
	}

	static bool tryParseFloat(StrView view, f32& outValue) {
		String_<256> tmp = view;
		auto ret = ::sscanf(tmp.c_str(), "%f", &outValue);
		if (ret != 1) return false;
		return true;
	}

	static bool tryParseFloat(StrView view, f64& outValue) {
		String_<256> tmp = view;
		auto ret = ::sscanf(tmp.c_str(), "%lf", &outValue);
		if (ret != 1) return false;
		return true;
	}
};

int StringUtil::ignoreCaseCompare(StrView a, StrView b) {
	size_t n = Math::min(a.size(), b.size());
	for (int i = 0; i < n; i++) {
		int c = ignoreCaseCompare(a[i], b[i]);
		if (c != 0) return c;
	}
	if (a.size() < b.size()) return -1;
	if (a.size() > b.size()) return 1;
	return 0;
}

void StringUtil::getTokens(StrView view, Vector<StrView>& outList, char separator) {
	outList.clear();
	auto tmp = view;
	while (tmp) {
		auto pair = StringUtil::splitByChar(tmp, separator);
		outList.push_back(pair.first);
		tmp = pair.second;
	}
}

bool StringUtil::tryParse(StrView view, i8&  outValue) { return StringUtil_ParseHelper::tryParseInt(view, outValue); }
bool StringUtil::tryParse(StrView view, i16& outValue) { return StringUtil_ParseHelper::tryParseInt(view, outValue); }
bool StringUtil::tryParse(StrView view, i32& outValue) { return StringUtil_ParseHelper::tryParseInt(view, outValue); }
bool StringUtil::tryParse(StrView view, i64& outValue) { return StringUtil_ParseHelper::tryParseInt(view, outValue); }

bool StringUtil::tryParse(StrView view, u8&  outValue) { return StringUtil_ParseHelper::tryParseUInt(view, outValue); }
bool StringUtil::tryParse(StrView view, u16& outValue) { return StringUtil_ParseHelper::tryParseUInt(view, outValue); }
bool StringUtil::tryParse(StrView view, u32& outValue) { return StringUtil_ParseHelper::tryParseUInt(view, outValue); }
bool StringUtil::tryParse(StrView view, u64& outValue) { return StringUtil_ParseHelper::tryParseUInt(view, outValue); }

bool StringUtil::tryParse(StrView view, f32& outValue) { return StringUtil_ParseHelper::tryParseFloat(view, outValue); }
bool StringUtil::tryParse(StrView view, f64& outValue) { return StringUtil_ParseHelper::tryParseFloat(view, outValue); }

} // namespace sge
