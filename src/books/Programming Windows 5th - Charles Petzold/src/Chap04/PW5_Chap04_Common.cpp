#if SGE_OS_WINDOWS

#include "PW5_Chap04_Common.h"

namespace sge {

const DefineMark* MySysmetricsDM::s_getMark() {
	class DM : public DefineMark {
		static void s_eval(	int& v,
							const Item& item,
							const DefineMark::NativeUIWindow* obj = nullptr)
		{
			v = ::GetSystemMetrics(item.id);
		}

	public:
		DM() {
			static Item dm[] = {
				SGE_DEFINEMARK__ITEM(SM_CXSCREEN,			"Screen width in pixels"),
				SGE_DEFINEMARK__ITEM(SM_CYSCREEN,			"Screen hight in pixels"),
				SGE_DEFINEMARK__ITEM(SM_CXVSCROLL,			"scroll width"),
				SGE_DEFINEMARK__ITEM(SM_CYHSCROLL,          "Horizontal scroll height"),
				SGE_DEFINEMARK__ITEM(SM_CYCAPTION,          "Caption bar height"),
				SGE_DEFINEMARK__ITEM(SM_CXBORDER,           "Window border width"),
				SGE_DEFINEMARK__ITEM(SM_CYBORDER,           "Window border height"),
				SGE_DEFINEMARK__ITEM(SM_CXFIXEDFRAME,       "Dialog window frame width"),
				SGE_DEFINEMARK__ITEM(SM_CYFIXEDFRAME,       "Dialog window frame height"),
				SGE_DEFINEMARK__ITEM(SM_CYVTHUMB,           "Vertical scroll thumb height"),
				SGE_DEFINEMARK__ITEM(SM_CXHTHUMB,           "Horizontal scroll thumb width"),
				SGE_DEFINEMARK__ITEM(SM_CXICON,             "Icon width"),
				SGE_DEFINEMARK__ITEM(SM_CYICON,             "Icon height"),
				SGE_DEFINEMARK__ITEM(SM_CXCURSOR,           "Cursor width"),
				SGE_DEFINEMARK__ITEM(SM_CYCURSOR,           "Cursor height"),
				SGE_DEFINEMARK__ITEM(SM_CYMENU,             "Menu bar height"),
				SGE_DEFINEMARK__ITEM(SM_CXFULLSCREEN,       "Full screen client area width"),
				SGE_DEFINEMARK__ITEM(SM_CYFULLSCREEN,       "Full screen client area height"),
				SGE_DEFINEMARK__ITEM(SM_CYKANJIWINDOW,      "Kanji window height"),
				SGE_DEFINEMARK__ITEM(SM_MOUSEPRESENT,       "Mouse present flag"),
				SGE_DEFINEMARK__ITEM(SM_CYVSCROLL,          "Vertical scroll arrow height"),
				SGE_DEFINEMARK__ITEM(SM_CXHSCROLL,          "Horizontal scroll arrow width"),
				SGE_DEFINEMARK__ITEM(SM_DEBUG,              "ebug version flag"),
				SGE_DEFINEMARK__ITEM(SM_SWAPBUTTON,         "Mouse buttons swapped flag"),
				SGE_DEFINEMARK__ITEM(SM_CXMIN,              "inimum window width"),
				SGE_DEFINEMARK__ITEM(SM_CYMIN,              "inimum window height"),
				SGE_DEFINEMARK__ITEM(SM_CXSIZE,             "Min / Max / Close button width"),
				SGE_DEFINEMARK__ITEM(SM_CYSIZE,             "Min / Max / Close button height"),
				SGE_DEFINEMARK__ITEM(SM_CXSIZEFRAME,        "Window sizing frame width"),
				SGE_DEFINEMARK__ITEM(SM_CYSIZEFRAME,        "Window sizing frame height"),
				SGE_DEFINEMARK__ITEM(SM_CXMINTRACK,         "Minimum window tracking width"),
				SGE_DEFINEMARK__ITEM(SM_CYMINTRACK,         "Minimum window tracking height"),
				SGE_DEFINEMARK__ITEM(SM_CXDOUBLECLK,        "Double click x tolerance"),
				SGE_DEFINEMARK__ITEM(SM_CYDOUBLECLK,        "Double click y tolerance"),
				SGE_DEFINEMARK__ITEM(SM_CXICONSPACING,      "Horizontal icon spacing"),
				SGE_DEFINEMARK__ITEM(SM_CYICONSPACING,      "Vertical icon spacing"),
				SGE_DEFINEMARK__ITEM(SM_MENUDROPALIGNMENT,  "Left or right menu drop"),
				SGE_DEFINEMARK__ITEM(SM_PENWINDOWS,         "Pen extensions installed"),
				SGE_DEFINEMARK__ITEM(SM_DBCSENABLED,        "Double - Byte Char Set enabled"),
				SGE_DEFINEMARK__ITEM(SM_CMOUSEBUTTONS,      "Number of mouse buttons"),
				SGE_DEFINEMARK__ITEM(SM_SECURE,             "Security present flag"),
				SGE_DEFINEMARK__ITEM(SM_CXEDGE,             "3 - D border width"),
				SGE_DEFINEMARK__ITEM(SM_CYEDGE,             "3 - D border height"),
				SGE_DEFINEMARK__ITEM(SM_CXMINSPACING,       "Minimized window spacing width"),
				SGE_DEFINEMARK__ITEM(SM_CYMINSPACING,       "Minimized window spacing height"),
				SGE_DEFINEMARK__ITEM(SM_CXSMICON,           "Small icon width"),
				SGE_DEFINEMARK__ITEM(SM_CYSMICON,           "Small icon height"),
				SGE_DEFINEMARK__ITEM(SM_CYSMCAPTION,        "Small caption height"),
				SGE_DEFINEMARK__ITEM(SM_CXSMSIZE,           "Small caption button width"),
				SGE_DEFINEMARK__ITEM(SM_CYSMSIZE,           "Small caption button height"),
				SGE_DEFINEMARK__ITEM(SM_CXMENUSIZE,         "Menu bar button width"),
				SGE_DEFINEMARK__ITEM(SM_CYMENUSIZE,         "Menu bar button height"),
				SGE_DEFINEMARK__ITEM(SM_ARRANGE,            "How minimized windows arranged"),
				SGE_DEFINEMARK__ITEM(SM_CXMINIMIZED,        "Minimized window width"),
				SGE_DEFINEMARK__ITEM(SM_CYMINIMIZED,        "Minimized window height"),
				SGE_DEFINEMARK__ITEM(SM_CXMAXTRACK,         "Maximum draggable width"),
				SGE_DEFINEMARK__ITEM(SM_CYMAXTRACK,         "Maximum draggable height"),
				SGE_DEFINEMARK__ITEM(SM_CXMAXIMIZED,        "Width of maximized window"),
				SGE_DEFINEMARK__ITEM(SM_CYMAXIMIZED,        "Height of maximized window"),
				SGE_DEFINEMARK__ITEM(SM_NETWORK,            "Network present flag"),
				SGE_DEFINEMARK__ITEM(SM_CLEANBOOT,          "How system was booted"),
				SGE_DEFINEMARK__ITEM(SM_CXDRAG,             "Avoid drag x tolerance"),
				SGE_DEFINEMARK__ITEM(SM_CYDRAG,             "Avoid drag y tolerance"),
				SGE_DEFINEMARK__ITEM(SM_SHOWSOUNDS,         "Present sounds visually"),
				SGE_DEFINEMARK__ITEM(SM_CXMENUCHECK,        "Menu check - mark width"),
				SGE_DEFINEMARK__ITEM(SM_CYMENUCHECK,        "Menu check - mark height"),
				SGE_DEFINEMARK__ITEM(SM_SLOWMACHINE,        "Slow processor flag"),
				SGE_DEFINEMARK__ITEM(SM_MIDEASTENABLED,     "Hebrew and Arabic enabled flag"),
				SGE_DEFINEMARK__ITEM(SM_MOUSEWHEELPRESENT,  "Mouse wheel present flag"),
				SGE_DEFINEMARK__ITEM(SM_XVIRTUALSCREEN,     "Virtual screen x origin"),
				SGE_DEFINEMARK__ITEM(SM_YVIRTUALSCREEN,     "Virtual screen y origin"),
				SGE_DEFINEMARK__ITEM(SM_CXVIRTUALSCREEN,    "Virtual screen width"),
				SGE_DEFINEMARK__ITEM(SM_CYVIRTUALSCREEN,    "Virtual screen height"),
				SGE_DEFINEMARK__ITEM(SM_CMONITORS,          "Number of monitors"),
				SGE_DEFINEMARK__ITEM(SM_SAMEDISPLAYFORMAT,  "Same color format flag"),
			};
			set(dm, &s_eval);
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
	_hdc = ::GetDC(_hwnd);

	ScopedGetDC hdc(_hwnd);
	auto tm = GDI::createTextMetrics(hdc);
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
	drawNeeded(); // ::UpdateWindow(_hwnd);
}

void PW5_MyDefineMarkWindow::onUIMouseEvent(UIMouseEvent& ev) {
	if (ev.isScroll()) {
		auto d = ev.scroll * 0.015f;
		if (_clientRect.w < _hScrollInfo->rangeMax()) {
			_hScrollInfo->setPos(_hScrollInfo->pos() + static_cast<int>(_cxChar * d.x));
			drawNeeded();
		}
		if (_clientRect.h < _vScrollInfo->rangeMax()) {
			_vScrollInfo->setPos(_vScrollInfo->pos() + static_cast<int>(_cyChar * -d.y));
			drawNeeded();
		}
	}
}

void PW5_MyDefineMarkWindow::onDraw() {
	SGE_ASSERT(_dm != nullptr);

	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	const auto& sysmetrics = _dm->data();

	int offsetY;
	_vScrollInfo->getPos(_hwnd, offsetY);

	int offsetX;
	_hScrollInfo->getPos(_hwnd, offsetX);

	DefineMark* dm = constCast(_dm);

	int i = 0;
	int outValue;

	for (auto& item : sysmetrics) {
		int x = 0 - offsetX;
		int y = _cyChar * i - offsetY;

		hdc.Fmt_textOut(x, y, "{:03d} {}", i, item.name);

		x += 24 * _cxCaps;
		hdc.textOut(x, y, item.mark);

		hdc.setTextAlign(PW5_TextAlignmentOption::Right | PW5_TextAlignmentOption::Top);
		x += 40 * _cxChar;

		dm->invoke(outValue, item, this);
		hdc.Fmt_textOut(x, y, "{:5d}", outValue);

		hdc.setTextAlign(PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top); // reset text align
		++i;
	}
}

}

#endif