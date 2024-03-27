#pragma once
#if SGE_OS_WINDOWS

#include "PW5_Common.h"

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

#define PW5_StockLogicalObject_Brush_ENUM_LIST(E) \
	E(None,		= NULL_BRUSH)	\
	E(White,	= WHITE_BRUSH)	\
	E(LtGray,	= LTGRAY_BRUSH)	\
	E(Gray,		= GRAY_BRUSH)	\
	E(DkGray,	= DKGRAY_BRUSH)	\
	E(Black,	= BLACK_BRUSH)	\
//----
SGE_ENUM_CLASS(PW5_StockLogicalObject_Brush, u8)

// The stock pens are 1 pixel wide default
#define PW5_StockLogicalObject_Pen_ENUM_LIST(E) \
	E(None,		= NULL_PEN)	/* NULL_PEN is a pen that doesn't draw */	\
	E(White,	= WHITE_PEN) \
	E(Black,	= BLACK_PEN) /*default pen*/ \
//----
SGE_ENUM_CLASS(PW5_StockLogicalObject_Pen, u8)

#define PW5_PenStyle_ENUM_LIST(E) \
	E(None,			= PS_NULL)			\
	E(Solid,		= PS_SOLID)			\
	E(Dash,			= PS_DASH)			\
	E(Dot,			= PS_DOT)			\
	E(DashDot,		= PS_DASHDOT)		\
	E(DashDotDot,	= PS_DASHDOTDOT)	\
//----
SGE_ENUM_CLASS(PW5_PenStyle, u8)

#define PW5_HatchStyle_ENUM_LIST(E) \
	E(Horizontal,	= HS_HORIZONTAL)\
	E(Vertical,		= HS_VERTICAL)	\
	E(Fdiagonal,	= HS_FDIAGONAL)	\
	E(Bdiagonal,	= HS_BDIAGONAL)	\
	E(Cross,		= HS_CROSS)		\
	E(DiagCross,	= HS_DIAGCROSS)	\
//----
SGE_ENUM_CLASS(PW5_HatchStyle, u8)

#define PW5_MappingMode_ENUM_LIST(E) \
	E(None,			= 0)				\
	E(Text,			= MM_TEXT)			\
	E(LowMetric,	= MM_LOMETRIC)		\
	E(HighMetric,	= MM_HIMETRIC)		\
	E(LowEnglish,	= MM_LOENGLISH)		\
	E(HighEnglish,	= MM_HIENGLISH)		\
	E(Twips,		= MM_TWIPS)			\
	E(Isotropic,	= MM_ISOTROPIC)		\
	E(Anisotropic,	= MM_ANISOTROPIC)	\
//----
SGE_ENUM_CLASS(PW5_MappingMode, u8)

} // namespace sge

namespace sge {
namespace GDI {

	using DTFlag = PW5_DrawTextFormatFlag;

	static constexpr Color4f kWhite		{ 1,1,1,1 };
	static constexpr Color4f kBlack		{ 0,0,0,1 };
	static constexpr Color4f kRed		{ 1,0,0,1 };
	static constexpr Color4f kGreen		{ 0,1,0,1 };
	static constexpr Color4f kBlue		{ 0,0,1,1 };
	static constexpr Color4f kYellow	{ 1,1,0,1 };
	static constexpr Color4f kViolet	{ 1,0,1,1 };
	static constexpr Color4f kCyan		{ 0,1,1,1 };
	static constexpr Color4f kGray		{ 0.2f,0.2f,0.2f,1.f };
	
	inline bool textOut(const ::HDC& hdc, int x, int y, StrView str) {
		if (str.empty()) return false;
		auto s = UtfUtil::toStringW(str);
		return ::TextOut(hdc, x, y, s.c_str(), static_cast<int>(s.size()));
	}

	inline bool textOut(const ::HDC& hdc, const Vec2f& pt, StrView str)	{
		return GDI::textOut(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), str);
	}

	template<class... Args>
	inline bool Fmt_textOut(const ::HDC& hdc, int x, int y, Args&&... args) {
		auto tmpStr = Fmt(SGE_FORWARD(args)...);
		auto s = UtfUtil::toStringW(tmpStr);
		return ::TextOut(hdc, x, y, s.c_str(), static_cast<int>(s.size()));
	}

	inline int drawText(const ::HDC& hdc, int left, int top, int right, int bottom, StrView str, UINT flags) {
		if (str.empty()) return 0;
		::RECT ltrb { left, top, right, bottom };
		auto s = UtfUtil::toStringW(str);
		return ::DrawText(hdc, s.c_str(), static_cast<int>(str.size()), &ltrb, flags);
	}
	inline int drawText(const ::HDC& hdc, int left, int top, int right, int bottom, StrView str,
						 DTFlag flags = DTFlag::SingleLine | DTFlag::Center | DTFlag::VCenter)
	{
		return GDI::drawText(hdc, left, top, right, bottom, str, static_cast<UINT>(flags));
	}
	inline int drawText(const ::HDC& hdc, const ::RECT& ltrb, StrView str, DTFlag flags) {
		return drawText(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, str, flags);
	}
	inline int drawText(const ::HDC& hdc, const Rect2f& xywh, StrView str, DTFlag flags) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return drawText(hdc, ltrb, str, flags);
	}

	inline bool rectangle(const ::HDC& hdc, int left, int top, int right, int bottom) { return ::Rectangle(hdc, left, top, right, bottom); }
	inline bool rectangle(const ::HDC& hdc, const ::RECT& ltrb) { return ::Rectangle(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom); }
	inline bool rectangle(const ::HDC& hdc, const Rect2f& xywh) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return rectangle(hdc, ltrb);
	}

	inline bool ellipse(const ::HDC& hdc, int left, int top, int right, int bottom) {
		return ::Ellipse(hdc, left, top, right, bottom);
	}
	inline bool ellipse(const ::HDC& hdc, const ::RECT& ltrb) {
		return ::Ellipse(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom);
	}
	inline bool ellipse(const ::HDC& hdc, const Rect2f& xywh) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return ellipse(hdc, ltrb);
	}

	inline bool roundRect(const ::HDC& hdc, int left, int top, int right, int bottom,
						  int xEllipse = 0, int yEllipse = 0) {
		return ::RoundRect(hdc, left, top, right, bottom, xEllipse, yEllipse);
	}
	inline bool roundRect(const ::HDC& hdc, const ::RECT& ltrb,
						  int xEllipse = 0, int yEllipse = 0) {
		return ::RoundRect(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, xEllipse, yEllipse);
	}
	inline bool roundRect(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& cornerEllipseSize = Vec2f::s_zero()) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		return roundRect(hdc, ltrb, static_cast<int>(cornerEllipseSize.x), static_cast<int>(cornerEllipseSize.y));
	}

	inline bool arc(const ::HDC& hdc,
					int left, int top, int right, int bottom,
					int fromX, int fromY, int toX, int toY) {
		return ::Arc(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool arc(const ::HDC& hdc, const ::RECT& ltrb,
					int fromX, int fromY, int toX, int toY) {
		return ::Arc(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool arc(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return arc(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool chord(const ::HDC& hdc,
					  int left, int top, int right, int bottom,
					  int fromX, int fromY, int toX, int toY) {
		return ::Chord(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool chord(const ::HDC& hdc, const ::RECT& ltrb,
					  int fromX, int fromY, int toX, int toY) {
		return ::Chord(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool chord(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return chord(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool pie(const ::HDC& hdc,
					int left, int top, int right, int bottom,
					int fromX, int fromY, int toX, int toY) {
		return ::Pie(hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	inline bool pie(const ::HDC& hdc, const ::RECT& ltrb,
					int fromX, int fromY, int toX, int toY) {
		return ::Pie(hdc, ltrb.left, ltrb.top, ltrb.right, ltrb.bottom, fromX, fromY, toX, toY);
	}
	inline bool pie(const ::HDC& hdc, const Rect2f& xywh, const Vec2f& from, const Vec2f& to) {
		::RECT ltrb;
		Win32Util::convert(ltrb, xywh);
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		return pie(hdc, ltrb, iFrom.x, iFrom.y, iTo.x, iTo.y);
	}

	inline bool lineTo(const ::HDC& hdc, int x, int y) { return ::LineTo(hdc, x, y); }
	inline bool lineTo(const ::HDC& hdc, const ::POINT& pt)	{ return ::LineTo(hdc, pt.x, pt.y); }
	inline bool lineTo(const ::HDC& hdc, const Vec2f& pt) {
		return ::LineTo(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y));
	}
	inline bool lineTo(const ::HDC& hdc, const Vec2i& pt) { return ::LineTo(hdc, pt.x, pt.y); }

	inline void drawLine(const ::HDC& hdc, int fromX, int fromY, int toX, int toY) {
		::POINT lastPt;
		::MoveToEx(hdc, fromX, fromY, &lastPt);
		::LineTo(hdc, toX, toY);
		::MoveToEx(hdc, lastPt.x, lastPt.y, nullptr);
	}
	inline void drawLine(const ::HDC& hdc, const ::POINT& from, const ::POINT& to) {
		drawLine(hdc, from.x, from.y, to.x, to.y);
	}
	inline void drawLine(const ::HDC& hdc, const Vec2i& from, const Vec2i& to) {
		drawLine(hdc, from.x, from.y, to.x, to.y);
	}
	inline void drawLine(const ::HDC& hdc, const Vec2f& from, const Vec2f& to) {
		auto iFrom = Vec2i::s_cast(from);
		auto iTo = Vec2i::s_cast(to);
		drawLine(hdc, iFrom, iTo);
	}

	void drawPoint(const ::HDC& hdc, int x, int y, const Color4f& c = kBlack, int ptSize = 10);
	inline void drawPoint(const ::HDC& hdc, const ::POINT& pt, const Color4f& c, int ptSize) {
		drawPoint(hdc, pt.x, pt.y, c, ptSize);
	}
	inline void drawPoint(const ::HDC& hdc, const Vec2f& pt, const Color4f& c, int ptSize) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		drawPoint(hdc, pt_, c, ptSize);
	}

	inline ::COLORREF COLORREF_make(int r, int g, int b)	{ return RGB(r, g, b); }
	inline ::COLORREF COLORREF_make(const Color4b& c)		{ return RGB(c.r, c.g, c.b); }
	inline ::COLORREF COLORREF_make(const Color4f& c)		{ return RGB(c.r*255, c.g*255, c.b*255); }

	inline ::HBRUSH createSolidBrush(int r, int g, int b)	{ return ::CreateSolidBrush(COLORREF_make(r, g, b)); }
	inline ::HBRUSH createSolidBrush(const Color4b& c)		{ return ::CreateSolidBrush(COLORREF_make(c)); }
	inline ::HBRUSH createSolidBrush(const Color4f& c)		{ return ::CreateSolidBrush(COLORREF_make(c)); }

	inline void fillRect(const ::HDC& hdc, const ::RECT& rc, const ::HBRUSH& brush) {
		::SelectObject(hdc, brush);
		::FillRect(hdc, &rc, brush);
		::SelectObject(hdc, nullptr);
	}

	void drawDashedLine(const ::HDC& hdc, int fromX, int fromY, int toX, int toY, const Color4f& c = kBlack);
	inline void drawDashedLine(const ::HDC& hdc, const Vec2f& from, const Vec2f& to, const Color4f& c = kBlack) {
		auto iFrom = Vec2i::s_cast(from);
		auto iTo   = Vec2i::s_cast(to);
		drawDashedLine(hdc, iFrom.x, iFrom.y, iTo.x, iTo.y, c);
	}

	inline bool dPtoLP(const ::HDC& hdc, ::RECT& rect) {
		return ::DPtoLP(hdc, reinterpret_cast<::LPPOINT>(&rect), 2);
	}
	inline bool dPtoLP(const ::HDC& hdc, ::POINT& pt) {
		return ::DPtoLP(hdc, &pt, 1);
	}
	inline bool dPtoLP(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		bool ok = ::DPtoLP(hdc, &pt_, 1);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	inline bool lPtoDP(const ::HDC& hdc, ::RECT& rect) {
		return ::LPtoDP(hdc, reinterpret_cast<::LPPOINT>(&rect), 2);
	}
	inline bool lPtoDP(const ::HDC& hdc, ::POINT& pt) {
		return ::LPtoDP(hdc, &pt, 1);
	}
	inline bool lPtoDP(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		bool ok = ::LPtoDP(hdc, &pt_, 1);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	// device coordinates origin set/get
	inline bool setViewportOrg(const ::HDC& hdc, const ::POINT& pt) {
		return ::SetViewportOrgEx(hdc, pt.x, pt.y, nullptr);
	}
	inline bool setViewportOrg(const ::HDC& hdc, const Vec2f& pt) {
		return ::SetViewportOrgEx(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), nullptr);
	}
	inline bool setViewportOrg(const ::HDC& hdc, int x, int y) {
		return ::SetViewportOrgEx(hdc, x, y, nullptr);
	}

	inline bool getViewportOrg(const ::HDC& hdc, ::POINT& pt) {
		return ::GetViewportOrgEx(hdc, &pt);
	}
	inline bool getViewportOrg(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		bool ok = ::GetViewportOrgEx(hdc, &pt_);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	// logical coordinates origin set/get
	inline bool setWindowOrg(const ::HDC& hdc, const ::POINT& pt) {
		return ::SetWindowOrgEx(hdc, pt.x, pt.y, nullptr);
	}
	inline bool setWindowOrg(const ::HDC& hdc, const Vec2f& pt) {
		return ::SetWindowOrgEx(hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), nullptr);
	}
	inline bool setWindowOrg(const ::HDC& hdc, int x, int y) {
		return ::SetWindowOrgEx(hdc, x, y, nullptr);
	}

	inline bool getWindowOrg(const ::HDC& hdc, ::POINT& pt) {
		return ::GetWindowOrgEx(hdc, &pt);
	}
	inline bool getWindowOrg(const ::HDC& hdc, Vec2f& pt) {
		::POINT pt_;
		bool ok = ::GetWindowOrgEx(hdc, &pt_);
		Win32Util::convert(pt, pt_);
		return ok;
	}

	inline bool setViewportExt(const ::HDC& hdc, int x, int y) {
		return ::SetViewportExtEx(hdc, x, y, nullptr);
	}
	inline bool setViewportExt(const ::HDC& hdc, const ::SIZE& sz) {
		return ::SetViewportExtEx(hdc, sz.cx, sz.cy, nullptr);
	}
	inline bool setViewportExt(const ::HDC& hdc, const Vec2f& sz) {
		return ::SetViewportExtEx(hdc, static_cast<int>(sz.x), static_cast<int>(sz.y), nullptr);
	}

	inline bool setWindowExt(const ::HDC& hdc, int x, int y) {
		return ::SetWindowExtEx(hdc, x, y, nullptr);
	}
	inline bool setWindowExt(const ::HDC& hdc, const ::SIZE& sz) {
		return ::SetWindowExtEx(hdc, sz.cx, sz.cy, nullptr);
	}
	inline bool setWindowExt(const ::HDC& hdc, const Vec2f& sz) {
		return ::SetWindowExtEx(hdc, static_cast<int>(sz.x), static_cast<int>(sz.y), nullptr);
	}

	inline bool getViewportExt(const ::HDC& hdc, ::SIZE& sz) { return ::GetViewportExtEx(hdc, &sz); }
	inline bool getViewportExt(const ::HDC& hdc, Vec2f& sz) {
		::SIZE sz_;
		bool ok = ::GetViewportExtEx(hdc, &sz_);
		Win32Util::convert(sz, sz_);
		return ok;
	}

	inline bool getWindowExt(const ::HDC& hdc, ::SIZE& sz) { return ::GetWindowExtEx(hdc, &sz); }
	inline bool getWindowExt(const ::HDC& hdc, Vec2f& sz) {
		::SIZE sz_;
		bool ok = ::GetWindowExtEx(hdc, &sz_);
		Win32Util::convert(sz, sz_);
		return ok;
	}

	inline float getDisplayScaleRatio() {
		::POINT ptZero = { 0, 0 };
		::HMONITOR hMonitor = ::MonitorFromPoint(ptZero, MONITOR_DEFAULTTONULL);

		::MONITORINFOEX monitorInfo;
		g_bzero(monitorInfo);
		monitorInfo.cbSize = sizeof(monitorInfo);
		::GetMonitorInfo(hMonitor, &monitorInfo);

		auto displayResolutionScaledWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
		if (displayResolutionScaledWidth == 0) return 1.f; // fallback

		::DEVMODE devmode;
		g_bzero(devmode);
		devmode.dmSize = sizeof(devmode);
		::EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devmode);

		auto displayResolutionWidth = devmode.dmPelsWidth;
		if (displayResolutionWidth == 0) return 1.f; // fallback
		return static_cast<float>(displayResolutionWidth) / displayResolutionScaledWidth;
	}

} // namespace GDI
} // namespace sge


namespace sge {
#if 0
#pragma mark ========= TextMetrics ============
#endif
struct TextMetrics {
	TextMetrics(::HWND hwnd);
	TextMetrics(::HDC hdc);
	TextMetrics(const ::TEXTMETRIC& tm);

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
	void	_internal_init(::HDC hdc);
	void	_set(const ::TEXTMETRIC& tm);
};

#if 0
#pragma mark ========= Scoped HDC ============
#endif
class ScopedHDC_NoHWND : public NonCopyable {
public:
	using DTFlag = GDI::DTFlag;

	virtual ~ScopedHDC_NoHWND() = default;

	operator const ::HDC& () const { return _hdc; }

	auto setTextAlign(PW5_TextAlignmentOption flags) {
		// https://github.com/MicrosoftDocs/win32/blob/docs/desktop-src/gdi/text-formatting-attributes.md
		return ::SetTextAlign(_hdc, enumInt(flags));
	}

	auto setMappingMode(PW5_MappingMode flag) { return ::SetMapMode(_hdc, enumInt(flag)); }
	PW5_MappingMode getMappingMode() const { return static_cast<PW5_MappingMode>(::GetMapMode(_hdc)); }

	auto setViewportOrg(int x, int y)		{ return GDI::setViewportOrg(_hdc, x, y); }
	auto setViewportOrg(const Vec2f& pt)	{ return GDI::setViewportOrg(_hdc, pt); }
	auto getViewportOrg(Vec2f& pt)			{ return GDI::getViewportOrg(_hdc, pt); }

	auto setWindowOrg(int x, int y)			{ return GDI::setWindowOrg(_hdc, x, y); }
	auto setWindowOrg(const Vec2f& pt)		{ return GDI::setWindowOrg(_hdc, pt); }
	auto getWindowOrg(Vec2f& pt)			{ return GDI::getWindowOrg(_hdc, pt); }

	auto setViewportExt(int x, int y)		{ return GDI::setViewportExt(_hdc, x, y); }
	auto setViewportExt(const Vec2f& pt)	{ return GDI::setViewportExt(_hdc, pt); }
	auto getViewportExt(Vec2f& sz)			{ return GDI::getViewportExt(_hdc, sz); }

	auto setWindowExt(int x, int y)			{ return GDI::setWindowExt(_hdc, x, y); }
	auto setWindowExt(const Vec2f& pt)		{ return GDI::setWindowExt(_hdc, pt); }
	auto getWindowExt(Vec2f& sz)			{ return GDI::getWindowExt(_hdc, sz); }

	auto dPtoLP(Vec2f& pt)					{ return GDI::dPtoLP(_hdc, pt); }
	auto lPtoDP(Vec2f& pt)					{ return GDI::lPtoDP(_hdc, pt); }

	auto textOut(int x, int y, StrView str)		const { return GDI::textOut(_hdc, x, y, str); }
	auto textOut(const Vec2f& pt, StrView str)	const { return GDI::textOut(_hdc, pt, str); }

	auto drawText(int l, int t, int r, int b, StrView str) const {
		return GDI::drawText(_hdc, l, t, r, b, str);
	}
	auto drawText(int l, int t, int r, int b, StrView str, DTFlag flags) const {
		return GDI::drawText(_hdc, l, t, r, b, str, flags);
	}
	auto drawText(const ::RECT& ltrb, StrView str, DTFlag flags) const {
		return GDI::drawText(_hdc, ltrb, str, flags);
	}																			 
	auto drawText(const Rect2f& xywh, StrView str, DTFlag flags) const {
		return GDI::drawText(_hdc, xywh, str, flags);
	}

	void drawLine(int fromX, int fromY, int toX, int toY)	const { GDI::drawLine(_hdc, fromX, fromY, toX, toY); }
	void drawLine(const ::POINT& from, const ::POINT& to)	const { GDI::drawLine(_hdc, from, to); }
	void drawLine(const Vec2f& from, const Vec2f& to)		const { GDI::drawLine(_hdc, from, to); }
	void drawLine(const Vec2i& from, const Vec2i& to)		const { GDI::drawLine(_hdc, from, to); }

	auto rectangle(int left, int top, int right, int bottom) const { return GDI::rectangle(_hdc, left, top, right, bottom); }
	auto rectangle(const ::RECT& ltrb) const { return GDI::rectangle(_hdc, ltrb); }
	auto rectangle(const Rect2f& xywh) const { return GDI::rectangle(_hdc, xywh); }

	auto ellipse(int left, int top, int right, int bottom)	const { return GDI::ellipse(_hdc, left, top, right, bottom); }
	auto ellipse(const ::RECT& ltrb)						const { return GDI::ellipse(_hdc, ltrb); }
	auto ellipse(const Rect2f& xywh)						const { return GDI::ellipse(_hdc, xywh); }

	auto roundRect(	int left, int top, int right, int bottom,
					int xEllipse = 0, int yEllipse = 0)	const {
		return GDI::roundRect(_hdc, left, top, right, bottom, xEllipse, yEllipse);
	}
	auto roundRect(const ::RECT& ltrb, int xEllipse = 0, int yEllipse = 0) const {
		GDI::roundRect(_hdc, ltrb, xEllipse, yEllipse);
	}
	auto roundRect(const Rect2f& xywh, const Vec2f& ellipseSize) const {
		return GDI::roundRect(_hdc, xywh, ellipseSize);
	}

	auto arc(int left, int top, int right, int bottom,
			 int fromX, int fromY, int toX, int toY) const {
		GDI::arc(_hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	auto arc(const ::RECT& ltrb, int fromX, int fromY, int toX, int toY) const {
		return GDI::arc(_hdc, ltrb, fromX, fromY, toX, toY);
	}
	auto arc(const Rect2f& xywh, const Vec2f& from, const Vec2f& to) const {
		return GDI::arc(_hdc, xywh, from, to);
	}

	auto chord( int left, int top, int right, int bottom,
				int fromX, int fromY, int toX, int toY)	const {
		return GDI::chord(_hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	auto chord(const ::RECT& ltrb, int fromX, int fromY, int toX, int toY) const {
		return GDI::chord(_hdc, ltrb, fromX, fromY, toX, toY);
	}
	auto chord(const Rect2f& xywh, const Vec2f& from, const Vec2f& to) const {
		return GDI::chord(_hdc, xywh, from, to);
	}
	
	auto pie(int left, int top, int right, int bottom,
			 int fromX, int fromY, int toX, int toY) const {
		return GDI::pie(_hdc, left, top, right, bottom, fromX, fromY, toX, toY);
	}
	auto pie(const ::RECT& ltrb, int fromX, int fromY, int toX, int toY) const {
		return GDI::pie(_hdc, ltrb, fromX, fromY, toX, toY);
	}
	auto pie(const Rect2f& xywh, const Vec2f& from, const Vec2f& to) const {
		return GDI::pie(_hdc, xywh, from, to);
	}

	void drawPoint(int x, int y, const Color4f& c, int ptSize)		const { GDI::drawPoint(_hdc, x, y, c, ptSize); }
	void drawPoint(const ::POINT& pt, const Color4f& c, int ptSize)	const { GDI::drawPoint(_hdc, pt, c, ptSize); }
	void drawPoint(const Vec2f& pt, const Color4f& c, int ptSize)	const { GDI::drawPoint(_hdc, pt, c, ptSize); }

	template<class... Args>
	auto Fmt_textOut(int x, int y, Args&&... args) const {
		return GDI::Fmt_textOut(_hdc, x, y, SGE_FORWARD(args)...);
	}

	template<class... Args>
	auto Fmt_textOut(const Vec2f& pt, Args&&... args) const {
		return GDI::Fmt_textOut(_hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), SGE_FORWARD(args)...);
	}

	template<class... Args>
	auto Fmt_drawText(int x, int y, Args&&... args) const;

protected:
	::HDC	_hdc = nullptr;
};

class ScopedHDC_ : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	ScopedHDC_(const ::HWND& hwnd)
		: _hwnd(hwnd) {}

	void clearBg(PW5_StockLogicalObject_Brush flag = PW5_StockLogicalObject_Brush::White) {
		Vec2f oldViewportOrg;
		Vec2f oldWindowOrg;

		getViewportOrg(oldViewportOrg);
		getViewportOrg(oldWindowOrg);

		bool isNotLeftTop = oldViewportOrg != oldWindowOrg;
		if (isNotLeftTop) {
			setViewportOrg(0, 0);
			setWindowOrg(0, 0);
		}

		::RECT rc;
		::GetClientRect(_hwnd, &rc); //  GetClientRect (which is always in terms of device units)
		auto brush = GetStockBrush(enumInt(flag));
		GDI::fillRect(_hdc, rc, brush);

		if (isNotLeftTop) {
			setViewportOrg(oldViewportOrg);
			setWindowOrg(oldWindowOrg);
		}
	}

protected:
	const HWND& _hwnd;
};

class ScopedPaintStruct : public ScopedHDC_ {
	using Base = ScopedHDC_;
public:
	ScopedPaintStruct(const ::HWND& hwnd) : Base(hwnd) {
		_hdc = ::BeginPaint(hwnd, &_ps);
		if (!_hdc) {
			SGE_LOG_ERROR("ScopedPaintStruct BeginPaint");
		}
	}
	~ScopedPaintStruct() { ::EndPaint(_hwnd, &_ps); }

			::HDC	hdc()		const { return _ps.hdc;		}
			bool	fErase()	const { return _ps.fErase;	}
	const	::RECT&	rcPaint()	const { return _ps.rcPaint; }

private:
	::PAINTSTRUCT _ps;
};

class ScopedGetDC : public ScopedHDC_ {
	using Base = ScopedHDC_;
public:
	ScopedGetDC(const ::HWND& hwnd) : Base(hwnd) {
		_hdc = ::GetDC(hwnd);
		if (!_hdc) {
			SGE_LOG_ERROR("ScopedGetDC GetDC");
		}
	}
	~ScopedGetDC() {
		if (_hdc) {
			::ReleaseDC(_hwnd, _hdc);
			_hdc = nullptr;
		}
	}
};

class ScopedGetWindowDC : public ScopedHDC_ {
	using Base = ScopedHDC_;
public:
	ScopedGetWindowDC(const ::HWND& hwnd) : Base(hwnd) {
		_hdc = ::GetWindowDC(hwnd);
		if (!_hdc) {
			SGE_LOG_ERROR("ScopedGetWindowDC GetWindowDC");
		}
	}
	~ScopedGetWindowDC() {
		if (_hdc) {
			::ReleaseDC(_hwnd, _hdc);
			_hdc = nullptr;
		}
	}
};

class ScopedCreateDC : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	ScopedCreateDC( const wchar_t* pszDriver,
					const wchar_t* pszDevice = nullptr,
					const wchar_t* pszOutput = nullptr,
					const ::DEVMODEW* pData  = nullptr)
	{
		_hdc = ::CreateDC(pszDriver, pszDevice, pszOutput, pData);
		if (!_hdc) {
			SGE_LOG_ERROR("ScopedCreateDC CreateDC");
		}
	}

	~ScopedCreateDC() {
		if (_hdc) {
			if (!::DeleteDC(_hdc)) SGE_LOG_ERROR("~ScopedCreateDC DeleteDC");
			_hdc = nullptr;
		}
	}
};

class ScopedCreateIC : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	ScopedCreateIC( const wchar_t* pszDriver,
					const wchar_t* pszDevice = nullptr,
					const wchar_t* pszOutput = nullptr,
					const ::DEVMODEW* pData = nullptr)
	{
		_hdc = ::CreateIC(pszDriver, pszDevice, pszOutput, pData);
		if (!_hdc) {
			SGE_LOG_ERROR("ScopedCreateIC CreateIC");
		}
	}

	~ScopedCreateIC() {
		if (_hdc) {
			if (!::DeleteDC(_hdc)) SGE_LOG_ERROR("~ScopedCreateIC DeleteDC");
			_hdc = nullptr;
		}
	}
};

class ScopedCreateCompatibleDC : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	ScopedCreateCompatibleDC(::HDC srcHdc) {
		_hdc = ::CreateCompatibleDC(srcHdc);
	}
	~ScopedCreateCompatibleDC() {
		if (_hdc) {
			if (!::DeleteDC(_hdc)) SGE_LOG_ERROR("~ScopedCreateCompatibleDC DeleteDC");
			_hdc = nullptr;
		}
	}
};

class ScopedSaveDC : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	ScopedSaveDC(::HDC hdc) : _id(0) {
		_hdc = hdc;
		_id = ::SaveDC(hdc);
		if (!_id) {
			SGE_LOG_ERROR("ScopedSaveDC SaveDC");
		}
	}
	~ScopedSaveDC() {
		if (_id && _hdc) {
			if (!::RestoreDC(_hdc, _id)) {
				SGE_LOG_ERROR("~ScopedSaveDC RestoreDC");
				_hdc = nullptr;
				_id  = 0;
			}
		}
	} 
private:
	int _id = 0;
};

class ScopedHDCBrush_Base : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	~ScopedHDCBrush_Base() {
		if (_brush) {
			SGE_ASSERT(_hdc != nullptr);
			if (!DeleteBrush(_brush)) {
				SGE_LOG_ERROR("DeleteBrush: handle is not valid or is currently selected into a DC");
			}
			_brush = nullptr;
		}
	}

	operator const ::HBRUSH& () const { return _brush; }

protected:
	::HBRUSH _brush = nullptr;
};

class ScopedCreateSolidBrush : public ScopedHDCBrush_Base {
	using Base = ScopedHDCBrush_Base;
public:
	ScopedCreateSolidBrush(::HDC hdc, const Color4f& c = GDI::kBlack) {
		_hdc = hdc;
		_brush = ::CreateSolidBrush(GDI::COLORREF_make(c));
		if (!_brush) {
			SGE_LOG_ERROR("ScopedCreateSolidBrush CreateSolidBrush");
		} else {
			SelectBrush(hdc, _brush);
		}
	}
};

class ScopedCreateHatchBrush : public ScopedHDCBrush_Base {
	using Base = ScopedHDCBrush_Base;
public:
	ScopedCreateHatchBrush(::HDC hdc,
		const Color4f& c = GDI::kBlack,
		PW5_HatchStyle flag = PW5_HatchStyle::Horizontal)
	{
		_hdc = hdc;
		_brush = ::CreateHatchBrush(enumInt(flag), GDI::COLORREF_make(c));
	}
};

class ScopedExtCreatePen_Base : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
protected:
	void _internal_ctor(::HDC hdc, DWORD penStyle, const Color4f& c, DWORD width) {
		// These are not unreasonable rules, but they can be a little tricky sometimes
		_hdc = hdc;

		// For the PS_SOLID, PS_NULL, and PS_INSIDEFRAME styles, the width argument is the width of the pen.
			// An width value of 0 directs Windows to use one pixel for the pen width.
			// If you specify a dotted or dashed pen style with a physical width greater than 1, Windows will use a solid pen instead

		// When you select a pen into a newly created device context, save the handle to the pen that SelectObject returns
			// What is _lastHPen? If this is the first SelectObject call you've made since obtaining the device context,
			// _lastHPen is a handle to the BLACK_PEN(default) stock object
			// After this call (SelectPen),
			// any lines you draw will use this pen until you select another pen into the device context
			// or release the device context handle

		::LOGBRUSH logBrush = {};
		logBrush.lbColor = GDI::COLORREF_make(c);
		logBrush.lbStyle = PS_SOLID;
		_lastHPen = SelectPen(_hdc, ::ExtCreatePen(penStyle, width, &logBrush, 0, nullptr));
	}

	~ScopedExtCreatePen_Base() {
		if (_lastHPen) {
			SGE_ASSERT(_hdc != nullptr);
			// You can now select that pen into the device context
			// and delete the pen you create(the handle returned from this second SelectObject call) in one statement
			DeletePen(SelectPen(_hdc, _lastHPen));
			_lastHPen = nullptr;
		}
	}
private:
	::HPEN _lastHPen = nullptr;
};

template<DWORD STYLE>
class ScopedExtCreatePen_Dash_Dot : public ScopedExtCreatePen_Base {
public:
	ScopedExtCreatePen_Dash_Dot(::HDC hdc, const Color4f& c) {
		// If you specify a dotted or dashed pen style with a physical width greater than 1,
		// Windows will use a solid pen instead
		_internal_ctor(hdc, STYLE, c, 1);
	}
};

class ScopedExtCreatePen_Solid : public ScopedExtCreatePen_Base {
public:
	ScopedExtCreatePen_Solid(::HDC hdc, const Color4f& c, DWORD width = 1) {
		_internal_ctor(hdc, static_cast<DWORD>(PW5_PenStyle::Solid), c, width);
	}
};

using ScopedExtCreatePen_Dash		= ScopedExtCreatePen_Dash_Dot<static_cast<DWORD>(PW5_PenStyle::Dash)>;
using ScopedExtCreatePen_Dot		= ScopedExtCreatePen_Dash_Dot<static_cast<DWORD>(PW5_PenStyle::Dot)>;
using ScopedExtCreatePen_DashDot	= ScopedExtCreatePen_Dash_Dot<static_cast<DWORD>(PW5_PenStyle::DashDot)>;
using ScopedExtCreatePen_DashDotDot = ScopedExtCreatePen_Dash_Dot<static_cast<DWORD>(PW5_PenStyle::DashDotDot)>;


class ScopedCreatePen_Base : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
protected:
	void _internal_ctor(::HDC hdc, int iStyle, const Color4f& c, int width) {
		_hdc = hdc;
		_lastHPen = SelectPen(_hdc, ::CreatePen(iStyle, width, GDI::COLORREF_make(c)));
	}
	~ScopedCreatePen_Base() {
		if (_lastHPen) {
			SGE_ASSERT(_hdc != nullptr);
			DeletePen(SelectPen(_hdc, _lastHPen));
			_lastHPen = nullptr;
		}
	}
private:
	::HPEN _lastHPen;
};

template<size_t PS_XXX>
class ScopedCreatePen_Dash_Dot : public ScopedCreatePen_Base {
public:
	ScopedCreatePen_Dash_Dot(::HDC hdc, const Color4f& c) {
		_internal_ctor(hdc, PS_XXX, c, 0);
	}
};

class ScopedCreatePen_Solid : public ScopedCreatePen_Base {
public:
	ScopedCreatePen_Solid(::HDC hdc, const Color4f& c, DWORD width = 1) {
		_internal_ctor(hdc, PS_SOLID, c, width);
	}
};

using ScopedCreatePen_Dash			= ScopedExtCreatePen_Dash_Dot<PS_DASH>;
using ScopedCreatePen_Dot			= ScopedExtCreatePen_Dash_Dot<PS_DOT>;
using ScopedCreatePen_DashDot		= ScopedExtCreatePen_Dash_Dot<PS_DASHDOT>;
using ScopedCreatePen_DashDotDot	= ScopedExtCreatePen_Dash_Dot<PS_DASHDOTDOT>;

class ScopedCreateBrush_Base : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
protected:
	void _internal_ctor(::HDC hdc, ::LOGBRUSH& logBrush) {
		_hdc = hdc;
		_lastHBrush = SelectBrush(hdc, ::CreateBrushIndirect(&logBrush));
	}
	~ScopedCreateBrush_Base() {
		if (_lastHBrush) {
			SGE_ASSERT(_hdc != nullptr);
			DeleteBrush(SelectBrush(_hdc, _lastHBrush));
			_lastHBrush = nullptr;
		}
	}
private:
	::HBRUSH _lastHBrush;
};

class ScopedCreateBrush_Solid : public ScopedCreateBrush_Base {
public:
	ScopedCreateBrush_Solid(::HDC hdc, const Color4f& c) {
		::LOGBRUSH logBrush = {};
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = GDI::COLORREF_make(c);
		logBrush.lbHatch = 0;
		_internal_ctor(hdc, logBrush);
	}
};

class ScopedCreateBrush_Hollow : public ScopedCreateBrush_Base {
public:
	ScopedCreateBrush_Hollow(::HDC hdc) {
		::LOGBRUSH logBrush = {};
		logBrush.lbStyle = BS_HOLLOW;
		logBrush.lbColor = 0;
		logBrush.lbHatch = 0;
		_internal_ctor(hdc, logBrush);
	}
};

class ScopedCreateBrush_Hatched : public ScopedCreateBrush_Base {
public:
	ScopedCreateBrush_Hatched(::HDC hdc, const Color4f& c = GDI::kBlack, PW5_HatchStyle flag = PW5_HatchStyle::Horizontal) {
		::LOGBRUSH logBrush = {};
		logBrush.lbStyle = BS_HATCHED;
		logBrush.lbColor = GDI::COLORREF_make(c);
		logBrush.lbHatch = static_cast<ULONG_PTR>(flag);
		_internal_ctor(hdc, logBrush);
	}
};

class ScopedCreateBrush_Pattern : public ScopedCreateBrush_Base {
public:
	ScopedCreateBrush_Pattern(::HDC hdc, ::HBITMAP& hBitMap) {
		::LOGBRUSH logBrush = {};
		logBrush.lbStyle = BS_PATTERN;
		logBrush.lbColor = 0;
		logBrush.lbHatch = reinterpret_cast<ULONG_PTR>(hBitMap);
		_internal_ctor(hdc, logBrush);
	}
};

} // namespace sge

namespace sge {
namespace GDI {

	inline TextMetrics createTextMetrics(::HDC hdc) { return TextMetrics(hdc); }

	template<class... Args>
	inline void Fmt_drawText(HDC hdc, int x, int y, Args&&... args) {
		auto s = Fmt(SGE_FORWARD(args)...);
		auto tm = GDI::createTextMetrics(hdc);
		GDI::drawText(hdc, x, y, x + static_cast<int>(s.size() * tm.maxCharWidth), y + tm.aveCharHeight, s.view(), 0);
	}

} // namespace GDI
} // namespace sge


namespace sge {

template<class... Args> inline
auto ScopedHDC_NoHWND::Fmt_drawText(int x, int y, Args&&... args) const {
	return GDI::Fmt_drawText(_hdc, x, y, SGE_FORWARD(args)...);
}

} // namespace sge

#endif