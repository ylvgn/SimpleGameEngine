#if SGE_OS_WINDOWS

#include "PW5_SineWave.h"

namespace sge {

void PW5_SineWave::onCreate(CreateDesc& desc) {
	Base::onCreate(desc);
}

void PW5_SineWave::onDraw() {
	//_example1();
	//_example2();
	_example3();
}

void PW5_SineWave::_example1() {
	ScopedGetDC hdc(_hwnd);

	int width  = static_cast<int>(_clientRect.w);
	int height = static_cast<int>(_clientRect.h);

	for (int x = 0; x < width; x += 100) {
		::MoveToEx(hdc, x, 0, nullptr);
		::LineTo(hdc, x, height);
	}

	for (int y = 0; y < height; y += 100) {
		::MoveToEx(hdc, 0,  y, nullptr);
		::LineTo(hdc, width, y);
	}
}

void PW5_SineWave::_example2() {
	ScopedGetDC hdc(_hwnd);
	static ::POINT apt[5] = {
		100, 100,
		200, 100,
		200, 200,
		100, 200,
		100, 100
	};

#define SGE_CASE 2
	#if (1 == SGE_CASE)
		::MoveToEx(hdc, apt[0].x, apt[0].y, nullptr);
		for (int i = 1; i < 5; i++)
			::LineTo(hdc, apt[i].x, apt[i].y); // change curent potison to (apt[i].x, apt[i].y)
	#elif (2 == SGE_CASE)
		// Polyline doesn't use or change the current position
		::Polyline(hdc, apt, 5 /*sizeof(apt) / sizeof(::POINT)*/);
	#elif (3 == SGE_CASE)
		// change current positoin when MoveToEx, but PolylineTo do not change current position
		::MoveToEx(hdc, apt[0].x, apt[0].y, nullptr);
		::PolylineTo(hdc, apt + 1, 4);
	#endif
#undef SGE_CASE

	::POINT pt;
	::GetCurrentPositionEx(hdc, &pt);
	SGE_LOG("{}, {}", pt.x, pt.y);
}

void PW5_SineWave::_example3() {
	ScopedGetDC hdc(_hwnd);

	float width = _clientRect.w;
	float height = _clientRect.h;
	int halfHeight = static_cast<int>(height * 0.5f);

	::MoveToEx(hdc, 0, halfHeight, nullptr);
	::LineTo(hdc, static_cast<int>(width), halfHeight);

	for (int i = 0; i < kNum; ++i) {
		float dt = static_cast<float>(i) / kNum;
		float rad = Math::twoPI<float>() * dt;
		int x = static_cast<int>(width * dt);
		int y = static_cast<int>(halfHeight * (1 - Math::sin(rad))); //int y = static_cast<int>(halfHeight * -Math::sin(rad)) + halfHeight;
		points[i].x = x;
		points[i].y = y;
	}

	::Polyline(hdc, points, kNum);
}

}

#endif