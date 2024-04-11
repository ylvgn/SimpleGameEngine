#if SGE_OS_WINDOWS

#include "PW5_WhatSize.h"

namespace sge {

void PW5_WhatSize::onCreate(CreateDesc& desc) {
	_isFirstFrame = true;
	desc.ownDC = true;
	Base::onCreate(desc);
}

void PW5_WhatSize::onDraw() {
	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	if (_isFirstFrame) {
		_tm.create(hdc);
		_isFirstFrame = false;
	}

	ScopedSelectStockObject scopedFont(hdc, PW5_StockLogicalObject::SystemFixedFont);

	hdc.setMapMode(MapMode::Anisotropic);
	hdc.setWindowExt(1, 1);
	hdc.setViewportExt(_tm.aveCharWidth, _tm.aveCharHeight);

	static const char* szHeading = "Mapping Mode            Left   Right     Top  Bottom Unit";
	static const char* szUndLine = "------------            ----   -----     ---  ------ ------------";

	_framePos.set(1, 0);
	_framePos.y++; hdc.textOut(_framePos, szHeading);
	_framePos.y++; hdc.textOut(_framePos, szUndLine);
	
	_show(MapMode::Text);
	_show(MapMode::LowMetric);
	_show(MapMode::HighMetric);
	_show(MapMode::LowEnglish);
	_show(MapMode::HighEnglish);
	_show(MapMode::Twips);
}

void PW5_WhatSize::_show(MapMode v) {
	ScopedGetDC hdc(_hwnd);

	::RECT rect;
	Vec2f  winExt;
	{
		ScopedSaveDC scoped(hdc);
		hdc.setMapMode(v);
		hdc.getClientRectInLogical(rect);
		hdc.getWindowExt(winExt);
	}

	using SRC = MapMode;

	Vec2f winExtInMM {
		static_cast<float>(::GetDeviceCaps(hdc, HORZSIZE)),
		static_cast<float>(::GetDeviceCaps(hdc, VERTSIZE))
	};

	switch (v) {
		case SRC::Text:
			SGE_LOG("{} pixel", winExt); break;
		case SRC::LowMetric:
		case SRC::HighMetric:
			SGE_LOG("{} mm", winExtInMM / winExt); break;
		case SRC::LowEnglish:
		case SRC::HighEnglish:
		case SRC::Twips: {
			Vec2f winExtInInch = GDI::mmToInch(winExtInMM);
			SGE_LOG("{} inch", winExtInInch / winExt);
		} break;
	}

	TempString s;
	switch (v) {
		case SRC::Text:			FmtTo(s, "1 px");			break;
		case SRC::LowMetric:	FmtTo(s, "0.1 mm");			break;
		case SRC::HighMetric:	FmtTo(s, "0.01 mm");		break;
		case SRC::LowEnglish:	FmtTo(s, "0.01 inch");		break;
		case SRC::HighEnglish:	FmtTo(s, "0.001 inch");		break;
		case SRC::Twips:		FmtTo(s, "1/1440 inch");	break;
	}

//	hdc.Fmt_textOut(_framePos.x, _framePos.y, "{}", v);		// ok
//	hdc.Fmt_textOut(_framePos.x, _framePos.y, "{:s}", v);	// not ok

	_framePos.y++;
	hdc.Fmt_textOut(_framePos.x, _framePos.y,
		"{:<20s} {:7d} {:7d} {:7d} {:7d} {:>12s}", // "%-20s %7d %7d %7d %7d %s"
		enumStr(v), rect.left, rect.right, rect.top, rect.bottom, s.c_str()
	);
}

}

#endif // SGE_OS_WINDOWS