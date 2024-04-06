#pragma once

#if SGE_OS_WINDOWS

namespace sge {

#define PW5_TextAlignmentOption_ENUM_LIST(E) \
	E(Left,		= TA_LEFT)		\
	E(Right,	= TA_RIGHT)		\
	E(Top,		= TA_TOP)		\
	E(Bottom,	= TA_BOTTOM)	\
	E(Center,	= TA_CENTER)	\
	E(BaseLine, = TA_BASELINE)	\
//----
SGE_ENUM_DECLARE(PW5_TextAlignmentOption, UINT)
SGE_ENUM_ALL_OPERATOR(PW5_TextAlignmentOption)

#define PW5_DrawTextFormatFlag_ENUM_LIST(E)	\
	E(Left,				= DT_LEFT)				\
	E(Top,				= DT_TOP)				\
	E(Right,			= DT_RIGHT)				\
	E(Center,			= DT_CENTER)			\
	E(VCenter,			= DT_VCENTER)			\
	E(Bottom,			= DT_BOTTOM)			\
	E(WordBreak,		= DT_WORDBREAK)			\
	E(SingleLine,		= DT_SINGLELINE)		\
	E(ExpandTabs,		= DT_EXPANDTABS)		\
	E(TabStop,			= DT_TABSTOP)			\
	E(NoClip,			= DT_NOCLIP)			\
	E(ExternalLeading,	= DT_EXTERNALLEADING)	\
	E(CalcRect,			= DT_CALCRECT)			\
	E(NoPrefix,			= DT_NOPREFIX)			\
//----
SGE_ENUM_DECLARE(PW5_DrawTextFormatFlag, u16)
SGE_ENUM_ALL_OPERATOR(PW5_DrawTextFormatFlag)

struct MyTextMetrics {

	MyTextMetrics(::HDC hdc) {
		::TEXTMETRIC tm;
		::GetTextMetrics(hdc, &tm);
		_set(tm);
	}

	MyTextMetrics(const ::TEXTMETRIC& tm) { _set(tm); }

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

private:
	void _set(const ::TEXTMETRIC& tm) {
		height					= tm.tmHeight;
		ascent					= tm.tmAscent;
		descent					= tm.tmDescent;
		internalLeading			= tm.tmInternalLeading;
		maxCharWidth			= tm.tmMaxCharWidth;
		externalLeading			= tm.tmExternalLeading;
		aveCharWidth			= tm.tmAveCharWidth;

		isFixedPitch			= (tm.tmPitchAndFamily & 1) == 0;
		aveCharHeight			= height + externalLeading;
		aveCharWidthUpperCase   = isFixedPitch ? aveCharWidth : static_cast<int>(1.5f * aveCharWidth);
	}
};

} // namespace sge

namespace sge {
namespace GDI {

	inline auto setTextAlign(::HDC hdc, PW5_TextAlignmentOption flags) {
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

	inline MyTextMetrics createMyTextMetrics(::HDC hdc) { return MyTextMetrics(hdc); }

	template<class... Args>
	inline void Fmt_drawText(HDC hdc, int x, int y, Args&&... args) {
		auto s = Fmt(SGE_FORWARD(args)...);
		auto tm = GDI::createMyTextMetrics(hdc);
		GDI::drawText(hdc, x, y, x + static_cast<int>(s.size() * tm.maxCharWidth), y + tm.aveCharHeight, s.view(), 0);
	}

} // namespace GDI
} // namespace sge

#endif