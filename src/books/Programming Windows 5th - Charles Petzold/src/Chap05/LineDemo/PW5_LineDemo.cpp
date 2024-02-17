#if SGE_OS_WINDOWS

#include "PW5_LineDemo.h"

namespace sge {

void PW5_LineDemo::onDraw() {
	_example1();
}

void PW5_LineDemo::_example1() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	int cxClient = static_cast<int>(_clientRect.w);
	int cyClient = static_cast<int>(_clientRect.h);

	{
		hdc.rectangle(cxClient / 8, cyClient / 8,
			7 * cxClient / 8, 7 * cyClient / 8);
	}

	{
		hdc.drawLine(0, 0, cxClient, cyClient);
		hdc.drawLine(0, cyClient, cxClient, 0);
	}

	hdc.ellipse(cxClient / 8, cyClient / 8, 7 * cxClient / 8, 7 * cyClient / 8);

	GDI::roundRect(hdc, cxClient / 4, cyClient / 4, 3 * cxClient / 4, 3 * cyClient / 4, cxClient / 4, cyClient / 4);
}

}

#endif