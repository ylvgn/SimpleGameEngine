#if SGE_OS_WINDOWS

#include "PW5_MySysMets3.h"

namespace sge {

void PW5_MySysMets3::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	desc.hScrollBar = true;
	desc.vScrollBar = true;

	Base::onCreate(desc);
}

void PW5_MySysMets3::onShow() {
	ScopedGetDC hdc(_hwnd);
	auto tm = hdc.createTextMetrics();
	_cxChar = tm.aveCharWidth;
	_cxCaps = tm.aveCharWidthUpperCase;
	_cyChar = tm.aveCharHeight;

	_hScrollInfo->setStep(_cxChar);
	_vScrollInfo->setStep(_cyChar);
}

void PW5_MySysMets3::onClientRectChanged(const Rect2f& rc) {
	// WM_SIZE
	Base::onClientRectChanged(rc);

	int NUMLINES		 = static_cast<int>(g_sysmetricsCount);
	int contentMaxHeight = _cyChar * NUMLINES;
	int contentMaxWidth  = 24 * _cxCaps + 40 * _cxChar;

	_vScrollInfo->setRange(0, contentMaxHeight);
	_vScrollInfo->setPage(static_cast<UINT>(_clientRect.h));

	_hScrollInfo->setRange(0, contentMaxWidth);
	_hScrollInfo->setPage(static_cast<UINT>(_clientRect.w));
}

void PW5_MySysMets3::onUIScrollBarEvent(UIScrollBarEvent& ev) {
	::UpdateWindow(_hwnd); // drawNeeded();
}

void PW5_MySysMets3::onDraw() {
	ScopedGetDC hdc(_hwnd);

	int NUMLINES = static_cast<int>(g_sysmetricsCount);
	const auto& sysmetrics = g_sysmetrics;

	auto brush = static_cast<HBRUSH>(GetStockBrush(WHITE_BRUSH));
	::SelectObject(hdc, brush);
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	::FillRect(hdc, &rc, brush);

	int offsetY;
	_vScrollInfo->getPos(_hwnd, offsetY);

	int offsetX;
	_hScrollInfo->getPos(_hwnd, offsetX);
	
	for (int i = 0; i < NUMLINES; ++i) {
		int x = 0 - offsetX;
		int y = _cyChar * i - offsetY;

		StrViewW label(sysmetrics[i].szLabel);
		hdc.Fmt_textOut(x, y, "{:03d} {}", i, label);

		x += 24 * _cxCaps;
		hdc.textOut(x, y, sysmetrics[i].szDesc);

		hdc.setTextAlign(TextAlignment::Right | TextAlignment::Top);
		x += 40 * _cxChar;
		hdc.Fmt_textOut(x, y, "{:5d}", GetSystemMetrics(sysmetrics[i].iIndex));

		hdc.setTextAlign(TextAlignment::Left | TextAlignment::Top); // reset text align
	}
}

}

#endif // SGE_OS_WINDOWS