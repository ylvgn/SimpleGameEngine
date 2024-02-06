#if SGE_OS_WINDOWS

#include "PW5_DevCaps1.h"

namespace sge {

SGE_DEFINE_MARK_STRUCT(MyDevcapsDM)

template<>
const DefineMark* DefineMarkOf<MyDevcapsDM>() {
	class DM : public DefineMark {
	public:
		DM() {
			static Item dm[] = {
				SGE_DEFINE_MARK__ITEM(HORZSIZE,      L"Width in millimeters"),
				SGE_DEFINE_MARK__ITEM(VERTSIZE,      L"Height in millimeters"),
				SGE_DEFINE_MARK__ITEM(HORZRES,       L"Width in pixels"),
				SGE_DEFINE_MARK__ITEM(VERTRES,       L"Height in raster lines"),
				SGE_DEFINE_MARK__ITEM(BITSPIXEL,     L"Color bits per pixel"),
				SGE_DEFINE_MARK__ITEM(PLANES,        L"Number of color planes"),
				SGE_DEFINE_MARK__ITEM(NUMBRUSHES,    L"Number of device brushes"),
				SGE_DEFINE_MARK__ITEM(NUMPENS,       L"Number of device pens"),
				SGE_DEFINE_MARK__ITEM(NUMMARKERS,    L"Number of device markers"),
				SGE_DEFINE_MARK__ITEM(NUMFONTS,      L"Number of device fonts"),
				SGE_DEFINE_MARK__ITEM(NUMCOLORS,     L"Number of device colors"),
				SGE_DEFINE_MARK__ITEM(PDEVICESIZE,   L"Size of device structure"),
				SGE_DEFINE_MARK__ITEM(ASPECTX,       L"Relative width of pixel"),
				SGE_DEFINE_MARK__ITEM(ASPECTY,       L"Relative height of pixel"),
				SGE_DEFINE_MARK__ITEM(ASPECTXY,      L"Relative diagonal of pixel"),
				SGE_DEFINE_MARK__ITEM(LOGPIXELSX,    L"Horizontal dots per inch"),
				SGE_DEFINE_MARK__ITEM(LOGPIXELSY,    L"Vertical dots per inch"),
				SGE_DEFINE_MARK__ITEM(SIZEPALETTE,   L"Number of palette entries"),
				SGE_DEFINE_MARK__ITEM(NUMRESERVED,   L"Reserved palette entries"),
				SGE_DEFINE_MARK__ITEM(COLORRES,      L"Actual color resolution")
			};
			set(dm);
		}
	};

	static DM info;
	return &info;
}


void PW5_DevCaps1::onCreate(CreateDesc& desc) {
	_dmInfo = MyDevcapsDM::s_getMarkOf();
	Base::onCreate(desc);
}

void PW5_DevCaps1::onDraw() {
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
		hdc.Fmt_textOut(x, y, "{:5d}", ::GetDeviceCaps(hdc, sysmetrics[i].id));

		hdc.setTextAlign(TextAlignment::Left | TextAlignment::Top); // reset text align
		++i;
	}
}

}

#endif