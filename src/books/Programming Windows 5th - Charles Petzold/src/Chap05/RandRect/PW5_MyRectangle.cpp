#if SGE_OS_WINDOWS

#include "PW5_MyRectangle.h"

namespace sge {

void PW5_MyRectangle::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);
}

void PW5_MyRectangle::onDraw() {
//	_example1(); // draw rect
//	_example2(); // intersect rect
	_example3(); // union rect
}

void PW5_MyRectangle::onUIMouseEvent(UIMouseEvent& ev) {
	if (ev.isDown() || ev.isDragging()) {
		_rectPos.set(ev.pos);
		drawNeeded();
	}
}

void PW5_MyRectangle::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	auto brush = GDI::getStockObject(PW5_StockLogicalObject_Brush::LtGray);

	Rect2f rc { 50, 100, 100, 200 };
	::RECT rect;
	Win32Util::convert(rect, rc);

	{
		ScopedCreatePen_Dash dash(hdc, GDI::kbBlue);
		GDI::rectangle(hdc, rc);
	}

	{
		::OffsetRect(&rect, 120, 0);
		::FillRect(hdc, &rect, brush);
	}

	{
		::OffsetRect(&rect, 120, 0);
		::FrameRect(hdc, &rect, brush);
	}

	{
		::OffsetRect(&rect, 120, 0);
		SGE_LOG("rect1:{}", rect);

		::InvertRect(hdc, &rect);
		SGE_LOG("rect2:{}", rect);
	}
}

void PW5_MyRectangle::_example2() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	::RECT o;
	::RECT rc1{ 50, 100, 200, 300 };
	::RECT rc2{
		static_cast<int>(_rectPos.x),
		static_cast<int>(_rectPos.y),
		300,
		400
	};

#if 0
	::IntersectRect(&o, &rc1, &rc2);
	{
		ScopedCreatePen_Dash dash(hdc, GDI::kbBlue);
		GDI::rectangle(hdc, rc1);
		GDI::rectangle(hdc, rc2);
	}
#else
	Rect2f r1, r2;
	Win32Util::convert(r1, rc1);
	Win32Util::convert(r2, rc2);
	Win32Util::convert(o, r1.intersectRect(r2));
	{
		ScopedCreatePen_Dash dash(hdc, GDI::kbBlue);
		GDI::rectangle(hdc, r1);
		GDI::rectangle(hdc, r2);
	}
#endif

	::FillRect(hdc, &o, GDI::getStockObject(PW5_StockLogicalObject_Brush::Black));
}

void PW5_MyRectangle::_example3() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	::RECT o;
	::RECT rc1 { 50, 100, 200, 300 };
	::RECT rc2 { 
		static_cast<int>(_rectPos.x),
		static_cast<int>(_rectPos.y),
		static_cast<int>(_rectPos.x) + 100,
		static_cast<int>(_rectPos.y) + 200
	};

#if 0
	::UnionRect(&o, &rc1, &rc2);
	{
		ScopedCreatePen_Dash dash(hdc, GDI::kbBlue);
		GDI::rectangle(hdc, rc1);
		GDI::rectangle(hdc, rc2);
	}
	
#else
	Rect2f r1, r2;
	Win32Util::convert(r1, rc1);
	Win32Util::convert(r2, rc2);
	Win32Util::convert(o, r1.unionRect(r2));
	{
		ScopedCreatePen_Dash dash(hdc, GDI::kbBlue);
		GDI::rectangle(hdc, r1);
		GDI::rectangle(hdc, r2);
	}
#endif

	{
		OffsetRect(&o, 300, 0);
		ScopedCreatePen_Dash dash(hdc, GDI::kbRed);
		GDI::rectangle(hdc, o);
	}
}

}

#endif // SGE_OS_WINDOWS