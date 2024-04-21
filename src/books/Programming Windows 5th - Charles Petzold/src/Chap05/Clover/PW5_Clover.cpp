#if SGE_OS_WINDOWS

#include "PW5_Clover.h"

namespace sge {

void PW5_Clover::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	_rgn	= ::CreateRectRgn(0, 0, 0, 0);
	_lr		= ::CreateRectRgn(0, 0, 0, 0);
	_tb		= ::CreateRectRgn(0, 0, 0, 0);
	_rgnRe	= ::CreateRectRgn(0, 0, 0, 0);
	Base::onCreate(desc);
}

void PW5_Clover::_destroy() {
	if (_lr) {
		::DeleteObject(_lr);
		_lr = nullptr;
	}

	if (_tb) {
		::DeleteObject(_tb);
		_tb = nullptr;
	}
	if (_rgnRe) {
		::DeleteObject(_rgnRe);
		_rgnRe = nullptr;
	}
	if (_rgn) {
		::DeleteObject(_rgn);
		_rgn = nullptr;
	}
}

void PW5_Clover::onClientRectChanged(const Rect2f& rc) {
	Base::onClientRectChanged(rc);

	_clientRecthypotLen = static_cast<float>(_hypot(_clientRect.w * 0.5f, _clientRect.h * 0.5f));
	_calcClippingRegion();
}

void PW5_Clover::_calcClippingRegion() {
	ScopedGetDC hdc(_hwnd);	

	ScopedCreateEllipticRgnIndirect left
	(
		0,
		static_cast<int>(_clientRect.h / 3.f),
		static_cast<int>(_clientRect.w / 2.f),
		static_cast<int>(_clientRect.h * 2.f/3.f)
	);

	ScopedCreateEllipticRgnIndirect right
	(
		static_cast<int>(_clientRect.w / 2.f),
		static_cast<int>(_clientRect.h / 3.f),
		static_cast<int>(_clientRect.w),
		static_cast<int>(_clientRect.h * 2.f/3.f)
	);

	ScopedCreateEllipticRgnIndirect top
	(
		static_cast<int>(_clientRect.w / 3.f),
		0,
		static_cast<int>(_clientRect.w * 2.f / 3.f),
		static_cast<int>(_clientRect.h / 2.f)
	);

	ScopedCreateEllipticRgnIndirect bottom
	(
		static_cast<int>(_clientRect.w / 3.f),
		static_cast<int>(_clientRect.h / 2.f),
		static_cast<int>(_clientRect.w * 2.f / 3.f),
		static_cast<int>(_clientRect.h)
	);

	ScopedCreateRectRgnIndirect all
	(
		0,
		0,
		static_cast<int>(_clientRect.w),
		static_cast<int>(_clientRect.h)
	);

	GDI::combineRgn(_lr, left, right, GDI::CombineRgnStyle::Or);
	GDI::combineRgn(_tb, top, bottom, GDI::CombineRgnStyle::Or);
	GDI::combineRgn(_rgn, _lr, _tb, GDI::CombineRgnStyle::Xor);
	GDI::combineRgn(_rgnRe, all, _rgn, GDI::CombineRgnStyle::Diff);
}

void PW5_Clover::onDraw() {
	ScopedGetDC hdc(_hwnd);
	//hdc.clearBg(); clearBg(FillRect) is not work in this case
		// solution1: wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
		// solution2: calc the _rgnRe(allRgn - _rgn) and fill with white color (not perfectly)

	{
		ScopedSelectObject scoped(hdc, _rgn);
		hdc.setViewportOrg({ _clientRect.w * 0.5f, _clientRect.h * 0.5f });

		float pi2	= 2 * Math::PI<float>();
		float step	= pi2 / 360.f;

		Vec2f from	{ 0, 0 };
		Vec2f to	{ 0, 0 };

		for (float rad = 0; rad < pi2; rad += step) {
			float s, c;
			Math::sincos(rad, s, c);
			to.x = c * _clientRecthypotLen;
			to.y = s * _clientRecthypotLen;
			hdc.drawLine(from, to);
		}
	}

	{
		ScopedSelectObject scoped(hdc, _rgnRe);
		hdc.setViewportOrg(0,0);
		FillRgn(hdc, _rgnRe, GDI::getStockObject(GDI::StockObj_Brush::White));
	}
}

}

#endif // SGE_OS_WINDOWS