#if SGE_OS_WINDOWS

#include "PW5_Bezier.h"

namespace sge {

void PW5_Bezier::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);

	_cubicBezier.p1 = { _clientRect.w / 4,		_clientRect.h / 2 };
	_cubicBezier.c1 = { _clientRect.w / 2,		_clientRect.h / 3 };
	_cubicBezier.c2 = { _clientRect.w / 2,		_clientRect.h * 3 / 4 };
	_cubicBezier.p2 = { _clientRect.w * 3 / 4,	_clientRect.h / 2 };
}

void PW5_Bezier::onDraw() {
	//_example1();
	_example2();
}

void PW5_Bezier::onUIMouseEvent(UIMouseEvent& ev) {
	using Button = UIMouseEventButton;
	using Modifier = UIEventModifier;

	if (ev.isDragging()) {
		switch (ev.pressedButtons) {
			case Button::Left: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Shift)) {
					_cubicBezier.c1 = _mousePos;
				} else {
					_cubicBezier.p1 = _mousePos;
				}
				onDraw();
			}break;
			case Button::Right: {
				if (BitUtil::hasAny(ev.modifier, Modifier::Shift)) {
					_cubicBezier.c2 = _mousePos;
				} else {
					_cubicBezier.p2 = _mousePos;
				}
				onDraw();
			}break;
		}
	}
}

void PW5_Bezier::_example1() {
	const static int kNum = 31;
	static ::POINT points[kNum+1];

	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	for (int i = 0; i <= kNum; ++i) {
		float t = static_cast<float>(i) / kNum;
		auto p = _cubicBezier.lerp(t);
		Win32Util::convert(points[i], p);
	}

	_toPOINTs();
	::Polyline(hdc, points, kNum+1);
	_drawLines(hdc);
	_drawPoints(hdc);
}

void PW5_Bezier::_example2() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	_toPOINTs();
	::PolyBezier(hdc, _points.data(), static_cast<int>(_points.size()));
	_drawLines(hdc);
	_drawPoints(hdc);
}

}

#endif