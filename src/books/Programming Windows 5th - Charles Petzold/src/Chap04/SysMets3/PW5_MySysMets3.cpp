#if SGE_OS_WINDOWS

#include "PW5_MySysMets3.h"

namespace sge {

void PW5_MySysMets3::onCreate(CreateDesc& desc) {
	_dmInfo = MySysmetricsDM::s_getMarkOf();
	Base::onCreate(desc);
}

void PW5_MySysMets3::onDraw() {
	ScopedGetDC hdc(_hwnd);

	auto brush = static_cast<HBRUSH>(GetStockBrush(WHITE_BRUSH));
	::SelectObject(hdc, brush);
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	::FillRect(hdc, &rc, brush);

	int offsetY;
	_vScrollInfo->getPos(_hwnd, offsetY);

	int offsetX;
	_hScrollInfo->getPos(_hwnd, offsetX);

	const auto& sysmetrics = _dmInfo->data();
	int i = 0;
	for (auto& item : sysmetrics) {
		int x = 0 - offsetX;
		int y = _cyChar * i - offsetY;

		StrViewW label(item.name);
		hdc.Fmt_textOut(x, y, "{:03d} {}", i, label);

		x += 24 * _cxCaps;
		hdc.textOut(x, y, item.mark);

		hdc.setTextAlign(TextAlignment::Right | TextAlignment::Top);
		x += 40 * _cxChar;
		hdc.Fmt_textOut(x, y, "{:5d}", ::GetSystemMetrics(sysmetrics[i].id));

		hdc.setTextAlign(TextAlignment::Left | TextAlignment::Top); // reset text align
		++i;
	}
}

}

#endif // SGE_OS_WINDOWS