#pragma once

#if SGE_OS_WINDOWS

namespace sge {

#define PW5_TextAlignmentOption_ENUM_LIST(E) \
	E(Left,		= TA_LEFT		) \
	E(Right,	= TA_RIGHT		) \
	E(Top,		= TA_TOP		) \
	E(Bottom,	= TA_BOTTOM		) \
	E(Center,	= TA_CENTER		) \
	E(BaseLine, = TA_BASELINE	) \
//----
SGE_ENUM_DECLARE(PW5_TextAlignmentOption, UINT)
SGE_ENUM_ALL_OPERATOR(PW5_TextAlignmentOption)

#define PW5_DrawTextFormatFlag_ENUM_LIST(E) \
	E(Left,				= DT_LEFT			) \
	E(Top,				= DT_TOP			) \
	E(Right,			= DT_RIGHT			) \
	E(Center,			= DT_CENTER			) \
	E(VCenter,			= DT_VCENTER		) \
	E(Bottom,			= DT_BOTTOM			) \
	E(WordBreak,		= DT_WORDBREAK		) \
	E(SingleLine,		= DT_SINGLELINE		) \
	E(ExpandTabs,		= DT_EXPANDTABS		) \
	E(TabStop,			= DT_TABSTOP		) \
	E(NoClip,			= DT_NOCLIP			) \
	E(ExternalLeading,	= DT_EXTERNALLEADING) \
	E(CalcRect,			= DT_CALCRECT		) \
	E(NoPrefix,			= DT_NOPREFIX		) \
//----
SGE_ENUM_DECLARE(PW5_DrawTextFormatFlag, u16)
SGE_ENUM_ALL_OPERATOR(PW5_DrawTextFormatFlag)

struct MyTextMetrics {

	MyTextMetrics()
		: height(0)
		, ascent(0)
		, descent(0)
		, internalLeading(0)
		, maxCharWidth(0)
		, externalLeading(0)
		, aveCharWidth(0)
		, aveCharHeight(0)
		, aveCharWidthUpperCase(0)
		, isFixedPitch(false) {}

	MyTextMetrics(const ::TEXTMETRIC& r);

	void create(const ::TEXTMETRIC& tm);
	void create(const ::HWND& hwnd);
	void create(const ::HDC& hdc);

	int		height;
	int		ascent;
	int		descent;
	int		internalLeading;
	int		maxCharWidth;
	int		externalLeading;
	int		aveCharWidth;
	int		aveCharHeight;
	int		aveCharWidthUpperCase;
	bool	isFixedPitch : 1;
};

} // namespace sge


namespace sge {
namespace GDI {

	using DTFlag = PW5_DrawTextFormatFlag;
	using TAFlag = PW5_TextAlignmentOption;

	inline auto setTextAlign(::HDC hdc, TAFlag flags) {
		// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
		return ::SetTextAlign(hdc, enumInt(flags));
	}

	inline bool textOut(const ::HDC& hdc, int x, int y, StrView str) {
		if (str.empty()) return false;
		auto s = UtfUtil::toStringW(str);
		return ::TextOut(hdc, x, y, s.c_str(), static_cast<int>(s.size()));
	}
	inline bool textOut(const ::HDC& hdc, const Vec2f& pt, StrView str)	{
		return GDI::textOut(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), str);
	}
	inline bool textOut(const ::HDC& hdc, const Vec2i& pt, StrView str) {
		return GDI::textOut(hdc, pt.x, pt.y, str);
	}

	template<class... Args>
	inline bool Fmt_textOut(const ::HDC& hdc, int x, int y, Args&&... args) {
		auto tmpStr = Fmt(SGE_FORWARD(args)...);
		auto s = UtfUtil::toStringW(tmpStr);
		return ::TextOut(hdc, x, y, s.c_str(), static_cast<int>(s.size()));
	}

} // namespace GDI
} // namespace sge

#endif