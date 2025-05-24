#pragma once

#include "../base/sge_base.h"
#include "UtfUtil.h"
#include "Fmt.h"

namespace sge {

struct StringUtil {
	StringUtil() = delete;

	static void binToHex(String& out, Span<u8> data);

	static void appendBinToHex(String& out, ByteSpan data);
	
	static bool hasChar(StrView view, char ch) { return StrView::npos != view.find(ch); }

	static Pair<StrView, StrView> splitByChar	(StrView view, StrView seperators);
	static Pair<StrView, StrView> splitByChar	(StrView view, char seperator);
	static Pair<StrView, StrView> getLine		(StrView view);

	static StrView trimChar(StrView view, StrView seperators);

	static const char* findChar			(StrView view, StrView charList, bool ignoreCase);
	static const char* findCharFromEnd	(StrView view, StrView charList, bool ignoreCase);

	static int  ignoreCaseCompare(StrView a, StrView b);
	static int  ignoreCaseCompare(char a, char b)		{ return tolower(a) - tolower(b); }
	static int  ignoreCaseCompare(wchar_t a, wchar_t b) { return towlower(a) - towlower(b); }

	static constexpr const char* extractFromPrefix(const char* src, const char* prefix);

	static void getTokens(StrView view, Vector<StrView>& outList, char separator);

	static bool tryParse(StrView view, i8 & outValue);
	static bool tryParse(StrView view, i16& outValue);
	static bool tryParse(StrView view, i32& outValue);
	static bool tryParse(StrView view, i64& outValue);

	static bool tryParse(StrView view, u8 & outValue);
	static bool tryParse(StrView view, u16& outValue);
	static bool tryParse(StrView view, u32& outValue);
	static bool tryParse(StrView view, u64& outValue);

	static bool tryParse(StrView view, f32& outValue);
	static bool tryParse(StrView view, f64& outValue);
};
SGE_STATIC_ASSERT_NO_MEMBER_CLASS(StringUtil);

inline
Pair<StrView, StrView> StringUtil::splitByChar(StrView view, StrView seperators) {
	auto* s = view.begin();
	auto* e = view.end();
	for (auto* p = s; p < e; ++p) {
		if (hasChar(seperators, *p)) {
			auto r0 = StrView(s,   p-s);
			auto r1 = StrView(p+1, e-p-1);
			return {r0, r1};
		}
	}
	return { view, StrView() };
}

inline
Pair<StrView, StrView> StringUtil::splitByChar(StrView view, char seperator) {
	return splitByChar(view, StrView(seperator));
}

inline
Pair<StrView, StrView> StringUtil::getLine(StrView view) {
	return splitByChar(view, "\n");
}

inline
StrView StringUtil::trimChar(StrView view, StrView charList) {
	auto* p = view.begin();
	auto* e = view.end();
	for ( ; p < e; ++p) {
		if (!hasChar(charList, *p))
			break;
	}
	return StrView(p, e-p);
}

constexpr inline
const char* StringUtil::extractFromPrefix(const char* src, const char* prefix) {
	if (!src || !prefix) return "";
	auto srcLen = charStrlen(src);
	auto prefixLen = charStrlen(prefix);
	if (srcLen < prefixLen) return "";
	auto* p = src;
	auto* q = prefix;
	auto* qed = prefix + prefixLen;
	while (q < qed) {
		if (*q != *p) return "";
		++p; ++q;
	}
	return p;
}

} // namespace sge


inline
std::ostream& operator<<(std::ostream& s, const sge::StrView& v) {
	s.write(v.data(), v.size());
	return s;
}

template<>
struct fmt::formatter<sge::StrViewA> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const sge::StrViewA& v, fmt::format_context& ctx) {
		auto it = *ctx.out();
		for (const auto& c : v) {
			it = c;
			it++;
		}
		return ctx.out();
	}
};

template<>
struct fmt::formatter<sge::StrViewW> {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const sge::StrViewW& v, fmt::format_context& ctx) {
		sge::TempStringA tmp = sge::UtfUtil::toString(v);
		return fmt::format_to(ctx.out(), "{}", tmp);
	}
};

template<class T, size_t N, bool bEnableOverflow>
struct fmt::formatter<sge::StringT<T, N, bEnableOverflow> > {
	static auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	static auto format(const sge::StringT<T, N, bEnableOverflow>& v, fmt::format_context& ctx) {
		return fmt::format_to(ctx.out(), "{}", v.view());
	}
};