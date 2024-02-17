#if SGE_OS_WINDOWS

#include "PW5_Bezier.h"

namespace sge {

void PW5_Bezier::onCreate(CreateDesc& desc) {
	_points.resize(4);
	Base::onCreate(desc);

	int cxClient = static_cast<int>(_clientRect.w);
	int cyClient = static_cast<int>(_clientRect.h);

	_points[0] = { cxClient / 4,		cyClient / 2 };		// end point 1
	_points[1] = { cxClient / 2,		cyClient / 3 };		// control point 1
	_points[2] = { cxClient / 2,		cyClient * 3 / 4 }; // control point 2
	_points[3] = { cxClient * 3 / 4,	cyClient / 2 };		// end point 2
}

void PW5_Bezier::onDraw() {
	_example1();
}

void PW5_Bezier::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	::PolyBezier(hdc, _points.data(), static_cast<int>(_points.size()));

	hdc.drawLine(_points[0], _points[1]);
	hdc.drawLine(_points[3], _points[2]);
}

}

#endif