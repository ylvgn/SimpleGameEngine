#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_Bezier : public PW5_NativeUIWindow {
	using This = PW5_Bezier;
	using Base = PW5_NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
private:

	struct CubicBezier {
		Vec2f p1;
		Vec2f c1;
		Vec2f c2;
		Vec2f p2;

		Vec2f lerp(float t) const {
			t = Math::clamp01(t);
			float u   = 1 - t;
			float uu  = u * u;
			float uuu = uu * u;
			float tt  = t * t;
			float ttt = tt * t;
			return (p1 * uuu) + (c1 * 3 * t * uu) + (c2 * 3 * u * tt) + (p2 * ttt);
		}

		void toPOINTs(::POINT& p1_, ::POINT& c1_, ::POINT& c2_, ::POINT& p2_) {
			Win32Util::convert(p1_, p1);
			Win32Util::convert(c1_, c1);
			Win32Util::convert(c2_, c2);
			Win32Util::convert(p2_, p2);
		}
	};

	void _toPOINTs() {
		_points.resize(4);
		_cubicBezier.toPOINTs(_points[0], _points[1], _points[2], _points[3]);
	}

	void _drawLines(const HDC& hdc) {
		::Polyline(hdc, _points.data(), static_cast<int>(_points.size()));
	}

	void _drawPoints(const HDC& hdc) {
		GDI::drawPoint(hdc, static_cast<int>(_cubicBezier.p1.x), static_cast<int>(_cubicBezier.p1.y), GDI::kRed);
		GDI::drawPoint(hdc, static_cast<int>(_cubicBezier.c1.x), static_cast<int>(_cubicBezier.c1.y), GDI::kGreen);
		GDI::drawPoint(hdc, static_cast<int>(_cubicBezier.c2.x), static_cast<int>(_cubicBezier.c2.y), GDI::kBlue);
		GDI::drawPoint(hdc, static_cast<int>(_cubicBezier.p2.x), static_cast<int>(_cubicBezier.p2.y), GDI::kViolet);
	}

	void _example1();
	void _example2();

	Vector<::POINT, 4>	_points;
	CubicBezier			_cubicBezier;
};

}

#endif // SGE_OS_WINDOWS