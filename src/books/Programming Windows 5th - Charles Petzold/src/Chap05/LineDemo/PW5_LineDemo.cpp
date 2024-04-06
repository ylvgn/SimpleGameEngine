#if SGE_OS_WINDOWS

#include "PW5_LineDemo.h"

namespace sge {

void PW5_LineDemo::onDraw() {
	//_example1();
	_example2();
}

void PW5_LineDemo::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	int cxClient = static_cast<int>(_clientRect.w);
	int cyClient = static_cast<int>(_clientRect.h);

	{
		hdc.rectangle(cxClient / 8, cyClient / 8, 7 * cxClient / 8, 7 * cyClient / 8);
	}

	{
		hdc.drawLine(0, 0, cxClient, cyClient);
		hdc.drawLine(0, cyClient, cxClient, 0);
	}

	{
		hdc.ellipse( cxClient / 8, cyClient / 8, 7 * cxClient / 8, 7 * cyClient / 8);
	}

	{
		hdc.roundRect(cxClient / 4, cyClient / 4, 3 * cxClient / 4, 3 * cyClient / 4, cxClient / 4, cyClient / 4);
	}
}

void PW5_LineDemo::_example2() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	float halfW = _clientRect.w * 0.5f;
	float halfH = _clientRect.h * 0.5f;

	{ // four quadrant with dashed lines
		GDI::drawDashedLine(hdc, { 0, halfH }, { _clientRect.w, halfH }, GDI::kbRed);
		GDI::drawDashedLine(hdc, { halfW, 0 }, { halfW, _clientRect.h }, GDI::kbRed);
	}

	{ // drawing an arc in a counterclockwise: from -> to in counterclockwise around arc
		{
			hdc.arc(_clientRect, { halfW, 0.f }, { 0, halfH });
		}
		{
			// the points 'from' and 'to' is one of the point on the rectangle bounding box
			Rect2f rc = { 0, 0, halfW, halfH };
			hdc.arc(rc, { rc.w * 0.5f, 0 }, { 0, rc.h * 0.5f });
		}
	}

	{ // drawing a chord in a counterclockwise, like arc
		{
			hdc.chord(_clientRect, { 0.f, halfH }, { halfW, _clientRect.h });
		}
		{
			Rect2f rc = { 0, halfH, halfW, halfH };
			hdc.chord(rc, rc.pos + Vec2f(0, rc.h * 0.5f), rc.pos + Vec2f(rc.w * 0.5f, rc.h));
		}
	}

	{ // drawing a pie in a counterclockwise, like arc
		{
			hdc.pie(_clientRect, { _clientRect.w, halfH }, { halfW, 0 });
		}
		{
			Rect2f rc = { halfW, 0, halfW, halfH };
			hdc.pie(rc, rc.pos + Vec2f(rc.w, rc.h * 0.5f), rc.pos + Vec2f(rc.w * 0.5f, 0));
		}
	}
}

}

#endif // SGE_OS_WINDOWS