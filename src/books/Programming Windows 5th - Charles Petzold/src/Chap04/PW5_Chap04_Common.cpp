#if SGE_OS_WINDOWS

#include "PW5_Chap04_Common.h"

namespace sge {

const DefineMark* MySysmetricsDM::s_getMark() {
	class DM : public DefineMark {
		static void eval(int& v, const PW5_MyDefineMarkWindow* obj, const Item& item) {
			v = ::GetSystemMetrics(item.id);
		}
	public:
		DM() {
			static Item dm[] = {
				SGE_DEFINEMARK__ITEM(SM_CXSCREEN,			L"Screen width in pixels"),
				SGE_DEFINEMARK__ITEM(SM_CYSCREEN,			L"ight in pixels"),
				SGE_DEFINEMARK__ITEM(SM_CXVSCROLL,			L"scroll width"),
				SGE_DEFINEMARK__ITEM(SM_CYHSCROLL,          L"Horizontal scroll height"),
				SGE_DEFINEMARK__ITEM(SM_CYCAPTION,          L"Caption bar height"),
				SGE_DEFINEMARK__ITEM(SM_CXBORDER,           L"Window border width"),
				SGE_DEFINEMARK__ITEM(SM_CYBORDER,           L"Window border height"),
				SGE_DEFINEMARK__ITEM(SM_CXFIXEDFRAME,       L"Dialog window frame width"),
				SGE_DEFINEMARK__ITEM(SM_CYFIXEDFRAME,       L"Dialog window frame height"),
				SGE_DEFINEMARK__ITEM(SM_CYVTHUMB,           L"Vertical scroll thumb height"),
				SGE_DEFINEMARK__ITEM(SM_CXHTHUMB,           L"Horizontal scroll thumb width"),
				SGE_DEFINEMARK__ITEM(SM_CXICON,             L"Icon width"),
				SGE_DEFINEMARK__ITEM(SM_CYICON,             L"Icon height"),
				SGE_DEFINEMARK__ITEM(SM_CXCURSOR,           L"Cursor width"),
				SGE_DEFINEMARK__ITEM(SM_CYCURSOR,           L"Cursor height"),
				SGE_DEFINEMARK__ITEM(SM_CYMENU,             L"Menu bar height"),
				SGE_DEFINEMARK__ITEM(SM_CXFULLSCREEN,       L"Full screen client area width"),
				SGE_DEFINEMARK__ITEM(SM_CYFULLSCREEN,       L"Full screen client area height"),
				SGE_DEFINEMARK__ITEM(SM_CYKANJIWINDOW,      L"Kanji window height"),
				SGE_DEFINEMARK__ITEM(SM_MOUSEPRESENT,       L"Mouse present flag"),
				SGE_DEFINEMARK__ITEM(SM_CYVSCROLL,          L"Vertical scroll arrow height"),
				SGE_DEFINEMARK__ITEM(SM_CXHSCROLL,          L"Horizontal scroll arrow width"),
				SGE_DEFINEMARK__ITEM(SM_DEBUG,              L"ebug version flag"),
				SGE_DEFINEMARK__ITEM(SM_SWAPBUTTON,         L"Mouse buttons swapped flag"),
				SGE_DEFINEMARK__ITEM(SM_CXMIN,              L"inimum window width"),
				SGE_DEFINEMARK__ITEM(SM_CYMIN,              L"inimum window height"),
				SGE_DEFINEMARK__ITEM(SM_CXSIZE,             L"Min / Max / Close button width"),
				SGE_DEFINEMARK__ITEM(SM_CYSIZE,             L"Min / Max / Close button height"),
				SGE_DEFINEMARK__ITEM(SM_CXSIZEFRAME,        L"Window sizing frame width"),
				SGE_DEFINEMARK__ITEM(SM_CYSIZEFRAME,        L"Window sizing frame height"),
				SGE_DEFINEMARK__ITEM(SM_CXMINTRACK,         L"Minimum window tracking width"),
				SGE_DEFINEMARK__ITEM(SM_CYMINTRACK,         L"Minimum window tracking height"),
				SGE_DEFINEMARK__ITEM(SM_CXDOUBLECLK,        L"Double click x tolerance"),
				SGE_DEFINEMARK__ITEM(SM_CYDOUBLECLK,        L"Double click y tolerance"),
				SGE_DEFINEMARK__ITEM(SM_CXICONSPACING,      L"Horizontal icon spacing"),
				SGE_DEFINEMARK__ITEM(SM_CYICONSPACING,      L"Vertical icon spacing"),
				SGE_DEFINEMARK__ITEM(SM_MENUDROPALIGNMENT,  L"Left or right menu drop"),
				SGE_DEFINEMARK__ITEM(SM_PENWINDOWS,         L"Pen extensions installed"),
				SGE_DEFINEMARK__ITEM(SM_DBCSENABLED,        L"Double - Byte Char Set enabled"),
				SGE_DEFINEMARK__ITEM(SM_CMOUSEBUTTONS,      L"Number of mouse buttons"),
				SGE_DEFINEMARK__ITEM(SM_SECURE,             L"Security present flag"),
				SGE_DEFINEMARK__ITEM(SM_CXEDGE,             L"3 - D border width"),
				SGE_DEFINEMARK__ITEM(SM_CYEDGE,             L"3 - D border height"),
				SGE_DEFINEMARK__ITEM(SM_CXMINSPACING,       L"Minimized window spacing width"),
				SGE_DEFINEMARK__ITEM(SM_CYMINSPACING,       L"Minimized window spacing height"),
				SGE_DEFINEMARK__ITEM(SM_CXSMICON,           L"Small icon width"),
				SGE_DEFINEMARK__ITEM(SM_CYSMICON,           L"Small icon height"),
				SGE_DEFINEMARK__ITEM(SM_CYSMCAPTION,        L"Small caption height"),
				SGE_DEFINEMARK__ITEM(SM_CXSMSIZE,           L"Small caption button width"),
				SGE_DEFINEMARK__ITEM(SM_CYSMSIZE,           L"Small caption button height"),
				SGE_DEFINEMARK__ITEM(SM_CXMENUSIZE,         L"Menu bar button width"),
				SGE_DEFINEMARK__ITEM(SM_CYMENUSIZE,         L"Menu bar button height"),
				SGE_DEFINEMARK__ITEM(SM_ARRANGE,            L"How minimized windows arranged"),
				SGE_DEFINEMARK__ITEM(SM_CXMINIMIZED,        L"Minimized window width"),
				SGE_DEFINEMARK__ITEM(SM_CYMINIMIZED,        L"Minimized window height"),
				SGE_DEFINEMARK__ITEM(SM_CXMAXTRACK,         L"Maximum draggable width"),
				SGE_DEFINEMARK__ITEM(SM_CYMAXTRACK,         L"Maximum draggable height"),
				SGE_DEFINEMARK__ITEM(SM_CXMAXIMIZED,        L"Width of maximized window"),
				SGE_DEFINEMARK__ITEM(SM_CYMAXIMIZED,        L"Height of maximized window"),
				SGE_DEFINEMARK__ITEM(SM_NETWORK,            L"Network present flag"),
				SGE_DEFINEMARK__ITEM(SM_CLEANBOOT,          L"How system was booted"),
				SGE_DEFINEMARK__ITEM(SM_CXDRAG,             L"Avoid drag x tolerance"),
				SGE_DEFINEMARK__ITEM(SM_CYDRAG,             L"Avoid drag y tolerance"),
				SGE_DEFINEMARK__ITEM(SM_SHOWSOUNDS,         L"Present sounds visually"),
				SGE_DEFINEMARK__ITEM(SM_CXMENUCHECK,        L"Menu check - mark width"),
				SGE_DEFINEMARK__ITEM(SM_CYMENUCHECK,        L"Menu check - mark height"),
				SGE_DEFINEMARK__ITEM(SM_SLOWMACHINE,        L"Slow processor flag"),
				SGE_DEFINEMARK__ITEM(SM_MIDEASTENABLED,     L"Hebrew and Arabic enabled flag"),
				SGE_DEFINEMARK__ITEM(SM_MOUSEWHEELPRESENT,  L"Mouse wheel present flag"),
				SGE_DEFINEMARK__ITEM(SM_XVIRTUALSCREEN,     L"Virtual screen x origin"),
				SGE_DEFINEMARK__ITEM(SM_YVIRTUALSCREEN,     L"Virtual screen y origin"),
				SGE_DEFINEMARK__ITEM(SM_CXVIRTUALSCREEN,    L"Virtual screen width"),
				SGE_DEFINEMARK__ITEM(SM_CYVIRTUALSCREEN,    L"Virtual screen height"),
				SGE_DEFINEMARK__ITEM(SM_CMONITORS,          L"Number of monitors"),
				SGE_DEFINEMARK__ITEM(SM_SAMEDISPLAYFORMAT,  L"Same color format flag"),
			};
			set(dm, eval);
		}
	};

	static DM dm;
	return &dm;
}

void PW5_MyDefineMarkWindow::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	desc.hScrollBar = true;
	desc.vScrollBar = true;

	Base::onCreate(desc);

	ScopedGetDC hdc(_hwnd);
	auto tm = hdc.createTextMetrics();
	_cxChar = tm.aveCharWidth;
	_cxCaps = tm.aveCharWidthUpperCase;
	_cyChar = tm.aveCharHeight;

	_hScrollInfo->setStep(_cxChar);
	_vScrollInfo->setStep(_cyChar);
}

void PW5_MyDefineMarkWindow::onClientRectChanged(const Rect2f& rc) {
	SGE_ASSERT(_dm != nullptr);

	// WM_SIZE
	Base::onClientRectChanged(rc);

	auto NUMLINES = static_cast<int>(_dm->data().size());

	int contentMaxHeight = _cyChar * NUMLINES;
	int contentMaxWidth  = 24 * _cxCaps + 40 * _cxChar;

	_vScrollInfo->setRange(0, contentMaxHeight);
	_vScrollInfo->setPage(static_cast<UINT>(_clientRect.h));

	_hScrollInfo->setRange(0, contentMaxWidth);
	_hScrollInfo->setPage(static_cast<UINT>(_clientRect.w));
}

void PW5_MyDefineMarkWindow::onUIScrollBarEvent(UIScrollBarEvent& ev) {
	::UpdateWindow(_hwnd); // drawNeeded();
}

void PW5_MyDefineMarkWindow::onDraw() {
	SGE_ASSERT(_dm != nullptr);

	ScopedGetDC hdc(_hwnd);

	const auto& sysmetrics = _dm->data();

	auto brush = static_cast<HBRUSH>(GetStockBrush(WHITE_BRUSH));
	::SelectObject(hdc, brush);
	::RECT rc;
	::GetClientRect(_hwnd, &rc);
	::FillRect(hdc, &rc, brush);

	int offsetY;
	_vScrollInfo->getPos(_hwnd, offsetY);

	int offsetX;
	_hScrollInfo->getPos(_hwnd, offsetX);

	DefineMark* dm = constCast(_dm);

	int i = 0;
	int out;

	for (auto& item : sysmetrics) {
		int x = 0 - offsetX;
		int y = _cyChar * i - offsetY;

		StrViewW label(item.name);
		hdc.Fmt_textOut(x, y, "{:03d} {}", i, label);

		x += 24 * _cxCaps;
		hdc.textOut(x, y, item.mark);

		hdc.setTextAlign(TextAlignment::Right | TextAlignment::Top);
		x += 40 * _cxChar;

		dm->io(out, this, item);
		hdc.Fmt_textOut(x, y, "{:5d}", out);

		hdc.setTextAlign(TextAlignment::Left | TextAlignment::Top); // reset text align
		++i;
	}
}

}

#endif