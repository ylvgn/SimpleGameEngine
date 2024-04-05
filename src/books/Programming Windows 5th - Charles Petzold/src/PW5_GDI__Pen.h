#pragma once

#include "PW5_GDI__HDC.h"

#if SGE_OS_WINDOWS

namespace sge {

	#define PW5_PenStyle_ENUM_LIST(E) \
	E(None,			= PS_NULL)			\
	E(Solid,		= PS_SOLID)			\
	E(Dash,			= PS_DASH)			\
	E(Dot,			= PS_DOT)			\
	E(DashDot,		= PS_DASHDOT)		\
	E(DashDotDot,	= PS_DASHDOTDOT)	\
//----
SGE_ENUM_CLASS(PW5_PenStyle, u8)

class ScopedExtCreatePen_Base : public ScopedHDC_NoHWND {
	using Base = ScopedHDC_NoHWND;
protected:
	void _internal_ctor(::HDC hdc, ::DWORD penStyle, const Color4b& c, ::DWORD width) {
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
	ScopedExtCreatePen_Dash_Dot(::HDC hdc, const Color4b& c) {
		// If you specify a dotted or dashed pen style with a physical width greater than 1,
		// Windows will use a solid pen instead
		_internal_ctor(hdc, STYLE, c, 1);
	}
};

class ScopedExtCreatePen_Solid : public ScopedExtCreatePen_Base {
public:
	ScopedExtCreatePen_Solid(::HDC hdc, const Color4b& c, DWORD width = 1) {
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

} // namespace sge


namespace sge {
namespace GDI {

	inline void drawDashedLine(const ::HDC& hdc, int fromX, int fromY, int toX, int toY, const Color4b& c = kbBlack) {
		ScopedExtCreatePen_Dash scoped(hdc, c);
		GDI::drawLine(hdc, fromX, fromY, toX, toY);
	}

	inline void drawDashedLine(const ::HDC& hdc, const Vec2f& from, const Vec2f& to, const Color4b& c = kbBlack) {
		auto iFrom = Vec2i::s_cast(from);
		auto iTo = Vec2i::s_cast(to);
		drawDashedLine(hdc, iFrom.x, iFrom.y, iTo.x, iTo.y, c);
	}

} // namespace GDI
} // namespace sge

#endif