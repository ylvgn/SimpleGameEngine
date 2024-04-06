#pragma once

#if SGE_OS_WINDOWS

#include "PW5_GDI__StockLogicalObject.h"
#include "PW5_GDI__Coordinate.h"
#include "PW5_GDI__Draw.h"

namespace sge {

class ScopedHDC_NoHWND : public NonCopyable {
public:
	using DTFlag = PW5_DrawTextFormatFlag;
	using MapMode = PW5_MapMode;
	using CoordinateDir = PW5_CoordinateDir;

	virtual ~ScopedHDC_NoHWND() = default;

	operator const ::HDC& () const { return _hdc; }

	auto setViewportOrg(int x, int y)		{ return GDI::setViewportOrg(_hdc, x, y); }
	auto setViewportOrg(const Vec2f& pt)	{ return GDI::setViewportOrg(_hdc, pt); }
	auto getViewportOrg(Vec2f& pt)			{ return GDI::getViewportOrg(_hdc, pt); }
	auto getViewportOrg(Vec2i& pt)			{ return GDI::getViewportOrg(_hdc, pt); }

	auto setWindowOrg(int x, int y)			{ return GDI::setWindowOrg(_hdc, x, y); }
	auto setWindowOrg(const Vec2f& pt)		{ return GDI::setWindowOrg(_hdc, pt); }
	auto getWindowOrg(Vec2f& pt)			{ return GDI::getWindowOrg(_hdc, pt); }
	auto getWindowOrg(Vec2i& sz)			{ return GDI::getWindowExt(_hdc, sz); }

	auto setViewportExt(int x, int y)		{ return GDI::setViewportExt(_hdc, x, y); }
	auto setViewportExt(const Vec2f& pt)	{ return GDI::setViewportExt(_hdc, pt); }
	auto getViewportExt(Vec2f& sz)			{ return GDI::getViewportExt(_hdc, sz); }
	auto getViewportExt(Vec2i& sz)			{ return GDI::getViewportExt(_hdc, sz); }

	auto setWindowExt(int x, int y)			{ return GDI::setWindowExt(_hdc, x, y); }
	auto setWindowExt(const Vec2f& pt)		{ return GDI::setWindowExt(_hdc, pt); }
	auto getWindowExt(Vec2f& sz)			{ return GDI::getWindowExt(_hdc, sz); }
	auto getWindowExt(Vec2i& sz)			{ return GDI::getWindowExt(_hdc, sz); }

	auto dPtoLP(Vec2f& pt)					{ return GDI::dPtoLP(_hdc, pt); }
	auto lPtoDP(Vec2f& pt)					{ return GDI::lPtoDP(_hdc, pt); }

	auto setMapMode(MapMode flag)			{ return GDI::setMapMode(_hdc, flag); }
	MapMode getMapMode() const				{ return GDI::getMapMode(_hdc); }

	CoordinateDir getCoordViewportDir() const { return GDI::getCoordinateDir(_hdc, true); }
	CoordinateDir getCoordWindowDir()	const { return GDI::getCoordinateDir(_hdc, false); }

	auto setTextAlign(PW5_TextAlignmentOption flags) { return GDI::setTextAlign(_hdc, flags); }

	auto textOut(int x, int y, StrView str)		const { return GDI::textOut(_hdc, x, y, str); }
	auto textOut(const Vec2f& pt, StrView str)	const { return GDI::textOut(_hdc, pt, str); }
	auto textOut(const Vec2i& pt, StrView str)	const { return GDI::textOut(_hdc, pt, str); }

	template<class... Args>
	auto Fmt_textOut(int x, int y, Args&&... args) const {
		return GDI::Fmt_textOut(_hdc, x, y, SGE_FORWARD(args)...);
	}
	template<class... Args>
	auto Fmt_textOut(const Vec2f& pt, Args&&... args) const {
		return GDI::Fmt_textOut(_hdc, static_cast<int>(pt.x), static_cast<int>(pt.y), SGE_FORWARD(args)...);
	}

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
	template<class... Args>
	auto Fmt_drawText(int x, int y, Args&&... args) const {
		return GDI::Fmt_drawText(_hdc, x, y, SGE_FORWARD(args)...);
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

	void drawPoint(int x, int y, const Color4b& c, int ptSize)		const;
	void drawPoint(const ::POINT& pt, const Color4b& c, int ptSize)	const;
	void drawPoint(const Vec2f& pt, const Color4b& c, int ptSize)	const;
	void drawPoint(const Vec2i& pt, const Color4b& c, int ptSize)	const;

protected:
	::HDC _hdc = nullptr;
};

class ScopedHDC_ : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	ScopedHDC_(const ::HWND& hwnd)
		: _hwnd(hwnd) {}

	void getClientRectInDevice(Rect2f& o) {
		::RECT rc;
		::GetClientRect(_hwnd, &rc);
		Win32Util::convert(o, rc);
	}

	void getClientRectInLogical(::RECT& o) {
		::GetClientRect(_hwnd, &o); // GetClientRect (which is always in terms of device units)
		GDI::dPtoLP(_hdc, o);
	}

	void getClientRectInLogical(Rect2f& o) {
		::RECT rc;
		getClientRectInLogical(rc);
		Win32Util::convert(o, rc);
	}

	void clearBg(PW5_StockLogicalObject_Brush flag = PW5_StockLogicalObject_Brush::White) {
		::RECT rc;
		getClientRectInLogical(rc);

		::HBRUSH brush;
		GDI::getStockObject(brush, flag);

		GDI::fillRect(_hdc, rc, brush); // FillRect is used in logical coordinates
	}

	void clearBg(const Color4b& color);

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

} // namespace sge

#endif