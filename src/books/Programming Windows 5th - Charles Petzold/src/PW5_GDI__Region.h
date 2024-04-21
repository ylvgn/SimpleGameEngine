#pragma once

#if SGE_OS_WINDOWS

#include "PW5_GDI__Object.h"

namespace sge {

#define PW5_CombineRgnStyle_ENUM_LIST(E) \
	E(And,	= RGN_AND  ) \
	E(Or,	= RGN_OR   ) \
	E(Xor,	= RGN_XOR  ) \
	E(Diff,	= RGN_DIFF ) \
	E(Copy,	= RGN_COPY ) \
	\
	E(Min,	= RGN_MIN  ) \
	E(Max,	= RGN_MAX  ) \
//----
SGE_ENUM_DECLARE(PW5_CombineRgnStyle, int)
SGE_ENUM_ALL_OPERATOR(PW5_CombineRgnStyle)

#define PW5_RegionFlag_ENUM_LIST(E) \
	E(RGNerror,	= RGN_ERROR		) \
	E(Null,		= NULLREGION	) \
	E(Simple,	= SIMPLEREGION  ) \
	E(Complex,	= COMPLEXREGION ) \
//----
SGE_ENUM_DECLARE(PW5_RegionFlag, int)
SGE_ENUM_ALL_OPERATOR(PW5_RegionFlag)

#define PW5_PolyFillMode_ENUM_LIST(E) \
	E(Alternate,= ALTERNATE ) \
	E(Winding,	= WINDING   ) \
//----
SGE_ENUM_DECLARE(PW5_PolyFillMode, int)

namespace GDI {
	
	using PolyFillMode		= PW5_PolyFillMode;
	using CombineRgnStyle	= PW5_CombineRgnStyle;
	using RegionFlag		= PW5_RegionFlag;

	inline RegionFlag combineRgn(::HRGN& dst, const ::HRGN& srcL, const ::HRGN& srcR, CombineRgnStyle flag) {
		// dst, srcL, srcR must be created region!!
		auto res = static_cast<RegionFlag>(::CombineRgn(dst, srcL, srcR, enumInt(flag)));
		SGE_ASSERT(res != GDI::RegionFlag::RGNerror);
		return res;
	}

} // namespace GDI
} // namespace sge

namespace sge {

class MyRgn_Base : public NonCopyable {
public:
	virtual ~MyRgn_Base() {
		if (_rgn) {
			::DeleteObject(_rgn);
			_rgn = nullptr;
		}
	}

	operator const ::HRGN& () const { return _rgn; }

protected:
	::HRGN _rgn = nullptr;
};

class ScopedCreateRectRgnIndirect : public MyRgn_Base {
public:
	ScopedCreateRectRgnIndirect(int l, int t, int r, int b) {
		::RECT rc { l, t, r, b };
		_rgn = ::CreateRectRgnIndirect(&rc);
	}
	ScopedCreateRectRgnIndirect(const Rect2f& rc) {
		::RECT rc_;
		Win32Util::convert(rc_, rc);
		_rgn = ::CreateRectRgnIndirect(&rc_);
	}
	ScopedCreateRectRgnIndirect(const ::RECT* rc) {
		_rgn = ::CreateRectRgnIndirect(rc);
	}
};

class ScopedCreateEllipticRgnIndirect : public MyRgn_Base {
public:
	ScopedCreateEllipticRgnIndirect(int l, int t, int r, int b) {
		::RECT rc { l, t, r, b };
		_rgn = ::CreateEllipticRgnIndirect(&rc);
	}
	ScopedCreateEllipticRgnIndirect(const Rect2f& rc) {
		::RECT rc_;
		Win32Util::convert(rc_, rc);
		_rgn = ::CreateEllipticRgnIndirect(&rc_);
	}
	ScopedCreateEllipticRgnIndirect(const ::RECT* rc) {
		_rgn = ::CreateEllipticRgnIndirect(rc);
	}
};

class ScopedHDCRgn_Base : public ScopedSelectObject {
	using Base = ScopedSelectObject;
public:
	~ScopedHDCRgn_Base() {
		if (_rgn) {
			::DeleteObject(_rgn);
			_rgn = nullptr;
		}
	}

	auto invertRgn()	{ return ::InvertRgn(_hdc, _rgn); }
	auto paintRgn()		{ return ::PaintRgn(_hdc, _rgn); }

	operator const ::HRGN& () const { return _rgn; }

protected:
	ScopedHDCRgn_Base(const ::HDC& hdc, const ::HRGN& rgn) : Base(hdc, rgn) {
		_rgn = rgn;
	}

	void _fillRgn(const ::HBRUSH& brush) { ::FillRgn(_hdc, _rgn, brush); }

	::HRGN _rgn = nullptr;
};

class ScopedSelectObject_RectRgn : public ScopedHDCRgn_Base {
	using Base = ScopedHDCRgn_Base;
	using This = ScopedSelectObject_RectRgn;
public:
	This(const ::HDC& hdc, int l, int t, int r, int b)
		: Base(hdc, ::CreateRectRgn(l, t, r, b)) {}
	This(const ::HDC& hdc, const Rect2i& rc)
		: This(hdc, rc.x, rc.y, rc.x + rc.w, rc.y + rc.h) {}
	This(const ::HDC& hdc, const ::RECT& rc)
		: This(hdc, rc.left, rc.top, rc.right, rc.bottom) {}

	void fillRgn(const ::HBRUSH& brush) { ::FillRgn(_hdc, _rgn, brush); }
};

class ScopedSelectObject_EllipticRgn : public ScopedHDCRgn_Base {
	using Base = ScopedHDCRgn_Base;
	using This = ScopedSelectObject_EllipticRgn;
public:
	This(const ::HDC& hdc, int l, int t, int r, int b)
		: Base(hdc, ::CreateEllipticRgn(l, t, r, b)) {}
	This(const ::HDC& hdc, const Rect2i& rc)
		: This(hdc, rc.x, rc.y, rc.x + rc.w, rc.y + rc.h) {}
	This(const ::HDC& hdc, const ::RECT& rc)
		: This(hdc, rc.left, rc.top, rc.right, rc.bottom) {}
};

class ScopedSelectObject_RoundRectRgn : public ScopedHDCRgn_Base {
	using Base = ScopedHDCRgn_Base;
	using This = ScopedSelectObject_RoundRectRgn;
public:
	This(const ::HDC& hdc, int l, int t, int r, int b, int w, int h)
		: Base(hdc, ::CreateRoundRectRgn(l, t, r, b, w, h)) {}
	This(const ::HDC& hdc, const Rect2i& rc, int w, int h)
		: This(hdc, rc.x, rc.y, rc.x + rc.w, rc.y + rc.h, w, h) {}
	This(const ::HDC& hdc, const ::RECT& rc, int w, int h)
		: This(hdc, rc.left, rc.top, rc.right, rc.bottom, w, h) {}
};

template<int MODE>
class ScopedCreatePolygonRgn_Base : public ScopedHDCRgn_Base {
	using Base = ScopedHDCRgn_Base;
public:
	ScopedCreatePolygonRgn_Base(const ::HDC& hdc, const ::POINT* pt, int ptCount)
		: Base(hdc, ::CreatePolygonRgn(pt, ptCount, MODE)) {}
};

class ScopedCreatePolygonRgn_Alternate : public ScopedCreatePolygonRgn_Base<enumInt(GDI::PolyFillMode::Alternate)> {
	using Base = ScopedCreatePolygonRgn_Base<enumInt(GDI::PolyFillMode::Alternate)>;
public:
	ScopedCreatePolygonRgn_Alternate(const ::HDC& hdc, const ::POINT& pt)
		: Base(hdc, &pt, 1) {}
};

class ScopedCreatePolygonRgn_Winding : public ScopedCreatePolygonRgn_Base<enumInt(GDI::PolyFillMode::Winding)> {
	using Base = ScopedCreatePolygonRgn_Base<enumInt(GDI::PolyFillMode::Winding)>;
public:
	ScopedCreatePolygonRgn_Winding(const ::HDC& hdc, const ::POINT& pt)
		: Base(hdc, &pt, 1) {}
};

}

#endif