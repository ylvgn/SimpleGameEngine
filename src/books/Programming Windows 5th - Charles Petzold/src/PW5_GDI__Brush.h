#pragma once

#if SGE_OS_WINDOWS

#include "PW5_GDI__HDC.h"

namespace sge {

#define PW5_HatchStyle_ENUM_LIST(E) \
	E(Horizontal,	= HS_HORIZONTAL)\
	E(Vertical,		= HS_VERTICAL)	\
	E(Fdiagonal,	= HS_FDIAGONAL)	\
	E(Bdiagonal,	= HS_BDIAGONAL)	\
	E(Cross,		= HS_CROSS)		\
	E(DiagCross,	= HS_DIAGCROSS)	\
//----
SGE_ENUM_CLASS(PW5_HatchStyle, u8)

class MyIndirectBrush_Base : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
public:
	struct CreateDesc {
		::HDC			hdc				= nullptr;
		Color4b			color			= GDI::kbBlack;
		PW5_HatchStyle	hatchFlag		= PW5_HatchStyle::Horizontal;
		::HBITMAP*		hBitMap			= nullptr;
		::LOGBRUSH&		_internal_used;
	};

	void create(CreateDesc& desc) {
		_hdc = desc.hdc;

		::LOGBRUSH logBrush;
		desc._internal_used = logBrush;
		onCreate(desc);

		::HBRUSH brush = ::CreateBrushIndirect(&logBrush);
		if (!brush)
			SGE_LOG_ERROR("MyIndirectBrush_Base: CreateBrushIndirect error");
		else
			_lastHBrush = SelectBrush(_hdc, brush);
	}

protected:
	virtual void onCreate(CreateDesc& desc) = 0;

	~MyIndirectBrush_Base() {
		if (_lastHBrush) {
			SGE_ASSERT(_hdc != nullptr);
			DeleteBrush(SelectBrush(_hdc, _lastHBrush));
			_lastHBrush = nullptr;
			_hdc = nullptr;
		}
	}

private:
	::HBRUSH _lastHBrush = nullptr;
};

class MyIndirectBrush_Solid : public MyIndirectBrush_Base {
protected:
	virtual void onCreate(CreateDesc& desc) override {
		desc._internal_used.lbStyle = BS_SOLID;
		desc._internal_used.lbColor = GDI::COLORREF_make(desc.color);
		desc._internal_used.lbHatch = 0;
	}
};

class MyIndirectBrush_Hollow : public MyIndirectBrush_Base {
protected:
	virtual void onCreate(CreateDesc& desc) override {
		desc._internal_used.lbStyle = BS_HOLLOW;
		desc._internal_used.lbColor = 0;
		desc._internal_used.lbHatch = 0;
	}
};

class MyIndirectBrush_Hatched : public MyIndirectBrush_Base {
protected:
	virtual void onCreate(CreateDesc& desc) override {
		desc._internal_used.lbStyle = BS_HATCHED;
		desc._internal_used.lbColor = GDI::COLORREF_make(desc.color);
		desc._internal_used.lbHatch = static_cast<ULONG_PTR>(desc.hatchFlag);
	}
};

class MyIndirectBrush_Pattern : public MyIndirectBrush_Base {
protected:
	virtual void onCreate(CreateDesc& desc) override {
		desc._internal_used.lbStyle = BS_PATTERN;
		desc._internal_used.lbColor = 0;
		desc._internal_used.lbHatch = reinterpret_cast<ULONG_PTR>(desc.hBitMap);
	}
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
	ScopedCreateSolidBrush(::HDC hdc, const Color4b& c = GDI::kbBlack) {
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

} // namespace sge


namespace sge {
namespace GDI {

	inline auto getObject(const ::HBRUSH& brush, ::LOGBRUSH& o) {
		// If you need to obtain information about a brush, you can call GetObject
		return GetObject(brush, sizeof(o), &o);
	}

	inline ::HBRUSH createSolidBrush(const Color4b& c)	{ return ::CreateSolidBrush(COLORREF_make(c)); }
	inline ::HBRUSH createSolidBrush(const Color4f& c)	{ return ::CreateSolidBrush(COLORREF_make(c)); }

	inline void drawPoint(const ::HDC& hdc, int x, int y, const Color4b& c = GDI::kbBlack, int ptSize = 10) {
		ScopedCreateSolidBrush brush(hdc, c);
		int halfSize = ptSize / 2;
		::RECT ltrb {
			x - halfSize,
			y - halfSize,
			x + halfSize,
			y + halfSize
		};
		::FillRect(hdc, &ltrb, brush);
	}
	inline void drawPoint(const ::HDC& hdc, const ::POINT& pt, const Color4b& c, int ptSize) {
		drawPoint(hdc, pt.x, pt.y, c, ptSize);
	}
	inline void drawPoint(const ::HDC& hdc, const Vec2f& pt, const Color4b& c, int ptSize) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		drawPoint(hdc, pt_, c, ptSize);
	}
	inline void drawPoint(const ::HDC& hdc, const Vec2i& pt, const Color4b& c, int ptSize) {
		::POINT pt_;
		Win32Util::convert(pt_, pt);
		drawPoint(hdc, pt_, c, ptSize);
	}

} // namespace GDI
} // namespace sge

#endif