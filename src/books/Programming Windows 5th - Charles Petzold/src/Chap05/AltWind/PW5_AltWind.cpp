#if SGE_OS_WINDOWS

#include "PW5_AltWind.h"

namespace sge {

void PW5_AltWind::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);
}

void PW5_AltWind::onDraw() {
	_example1();
}

void PW5_AltWind::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	static const int kPointCount = 10;
	static ::POINT aptFigure[kPointCount] = {
		10,70,
		50,70,
		50,10,
		90,10,
		90,50,
		30,50,
		30,90,
		70,90,
		70,30,
		10,30,
	};

	ScopedCreateSolidBrush brush(hdc, GDI::kGray);

	Vector<POINT, kPointCount> apt;
	apt.resize(kPointCount);

	float halfW = _clientRect.w / 2.f;
	for (int i = 0; i < kPointCount; i++) {
		apt[i].x = static_cast<LONG>(halfW * aptFigure[i].x / 100.f);
		apt[i].y = static_cast<LONG>(_clientRect.h * aptFigure[i].y / 100.f);
	}

	::SetPolyFillMode(hdc, ALTERNATE);
	::Polygon(hdc, apt.data(), kPointCount);

	for (int i = 0; i < kPointCount; i++) {
		apt[i].x += static_cast<LONG>(halfW);
	}
	::SetPolyFillMode(hdc, WINDING);
	::Polygon(hdc, apt.data(), kPointCount);
}

}

#endif