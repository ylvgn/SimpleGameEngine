#if SGE_OS_WINDOWS

#include "PW5_Chap04_Common.h"

namespace sge {

const PW5_MyDefinationRemarks* MySysmetricsDM::s_getRemarks() {

	class DM : public PW5_MyDefinationRemarks {
		using Base = PW5_MyDefinationRemarks;
		using Item = Base::Item;
		static void s_eval(	int& v, const Item& item,
							const Base::NativeUIWindow* obj = nullptr) {
			v = ::GetSystemMetrics(item.id);
		}
	public:
		DM() {
			static Item dm[] = {
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXSCREEN,			"Screen width in pixels"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYSCREEN,			"Screen hight in pixels"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXVSCROLL,			"scroll width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYHSCROLL,          "Horizontal scroll height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYCAPTION,          "Caption bar height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXBORDER,           "Window border width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYBORDER,           "Window border height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXFIXEDFRAME,       "Dialog window frame width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYFIXEDFRAME,       "Dialog window frame height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYVTHUMB,           "Vertical scroll thumb height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXHTHUMB,           "Horizontal scroll thumb width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXICON,             "Icon width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYICON,             "Icon height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXCURSOR,           "Cursor width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYCURSOR,           "Cursor height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMENU,             "Menu bar height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXFULLSCREEN,       "Full screen client area width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYFULLSCREEN,       "Full screen client area height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYKANJIWINDOW,      "Kanji window height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_MOUSEPRESENT,       "Mouse present flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYVSCROLL,          "Vertical scroll arrow height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXHSCROLL,          "Horizontal scroll arrow width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_DEBUG,              "ebug version flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_SWAPBUTTON,         "Mouse buttons swapped flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMIN,              "inimum window width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMIN,              "inimum window height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXSIZE,             "Min / Max / Close button width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYSIZE,             "Min / Max / Close button height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXSIZEFRAME,        "Window sizing frame width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYSIZEFRAME,        "Window sizing frame height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMINTRACK,         "Minimum window tracking width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMINTRACK,         "Minimum window tracking height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXDOUBLECLK,        "Double click x tolerance"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYDOUBLECLK,        "Double click y tolerance"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXICONSPACING,      "Horizontal icon spacing"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYICONSPACING,      "Vertical icon spacing"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_MENUDROPALIGNMENT,  "Left or right menu drop"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_PENWINDOWS,         "Pen extensions installed"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_DBCSENABLED,        "Double - Byte Char Set enabled"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CMOUSEBUTTONS,      "Number of mouse buttons"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_SECURE,             "Security present flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXEDGE,             "3 - D border width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYEDGE,             "3 - D border height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMINSPACING,       "Minimized window spacing width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMINSPACING,       "Minimized window spacing height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXSMICON,           "Small icon width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYSMICON,           "Small icon height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYSMCAPTION,        "Small caption height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXSMSIZE,           "Small caption button width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYSMSIZE,           "Small caption button height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMENUSIZE,         "Menu bar button width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMENUSIZE,         "Menu bar button height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_ARRANGE,            "How minimized windows arranged"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMINIMIZED,        "Minimized window width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMINIMIZED,        "Minimized window height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMAXTRACK,         "Maximum draggable width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMAXTRACK,         "Maximum draggable height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMAXIMIZED,        "Width of maximized window"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMAXIMIZED,        "Height of maximized window"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_NETWORK,            "Network present flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CLEANBOOT,          "How system was booted"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXDRAG,             "Avoid drag x tolerance"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYDRAG,             "Avoid drag y tolerance"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_SHOWSOUNDS,         "Present sounds visually"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXMENUCHECK,        "Menu check - mark width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYMENUCHECK,        "Menu check - mark height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_SLOWMACHINE,        "Slow processor flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_MIDEASTENABLED,     "Hebrew and Arabic enabled flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_MOUSEWHEELPRESENT,  "Mouse wheel present flag"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_XVIRTUALSCREEN,     "Virtual screen x origin"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_YVIRTUALSCREEN,     "Virtual screen y origin"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CXVIRTUALSCREEN,    "Virtual screen width"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CYVIRTUALSCREEN,    "Virtual screen height"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_CMONITORS,          "Number of monitors"),
				PW5_MYDEFINATIONREMARKS__ITEM(SM_SAMEDISPLAYFORMAT,  "Same color format flag"),
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
	_tm.create(_hdc);

	_hScrollInfo->setStep(_tm.aveCharWidth);
	_vScrollInfo->setStep(_tm.aveCharHeight);
}

void PW5_MyDefineMarkWindow::onClientRectChanged(const Rect2f& rc) {
	Base::onClientRectChanged(rc);

	SGE_ASSERT(_dm != nullptr);

	auto NUMLINES = static_cast<int>(_dm->data().size());

	int& cxChar = _tm.aveCharWidth;
	int& cxCaps = _tm.aveCharWidthUpperCase;
	int& cyChar = _tm.aveCharHeight;

	int contentMaxHeight = cyChar * NUMLINES;
	int contentMaxWidth  = 20 * cxCaps + 50 * cxChar;

	_vScrollInfo->setRange(0, contentMaxHeight);
	_vScrollInfo->setPage(static_cast<UINT>(_clientRect.h));

	_hScrollInfo->setRange(0, contentMaxWidth);
	_hScrollInfo->setPage(static_cast<UINT>(_clientRect.w));
}

void PW5_MyDefineMarkWindow::onUIScrollBarEvent(UIScrollBarEvent& ev) {
	drawNeeded();
}

void PW5_MyDefineMarkWindow::onUIMouseEvent(UIMouseEvent& ev) {
	if (ev.isScroll()) {
		auto d = ev.scroll * 0.015f;
		if (_clientRect.w < _hScrollInfo->_win32_rangeMax()) {
			_hScrollInfo->setPos(_hScrollInfo->_win32_pos() + static_cast<int>(_tm.aveCharWidth * d.x));
			drawNeeded();
		}
		if (_clientRect.h < _vScrollInfo->_win32_rangeMax()) {
			_vScrollInfo->setPos(_vScrollInfo->_win32_pos() + static_cast<int>(_tm.aveCharHeight * -d.y));
			drawNeeded();
		}
	}
}

void PW5_MyDefineMarkWindow::onUIKeyboardEvent(UIKeyboardEvent& ev) {
	using KeyCode = UIKeyboardEvent::KeyCode;

	if (ev.isDown(KeyCode::Home))		_vScrollInfo->scrollToHome();
	if (ev.isDown(KeyCode::End))		_vScrollInfo->scrollToEnd();
	if (ev.isDown(KeyCode::PageUp))		_vScrollInfo->scrollPageBackward();
	if (ev.isDown(KeyCode::PageDown))	_vScrollInfo->scrollPageForward();
	if (ev.isDown(KeyCode::UpArrow))	_vScrollInfo->scrollStepBackward();
	if (ev.isDown(KeyCode::DownArrow))	_vScrollInfo->scrollStepForward();
	if (ev.isDown(KeyCode::LeftArrow))	_hScrollInfo->scrollStepBackward();
	if (ev.isDown(KeyCode::RightArrow))	_hScrollInfo->scrollStepForward();
}

void PW5_MyDefineMarkWindow::onDraw() {
	SGE_ASSERT(_dm != nullptr);

	ScopedGetDC hdc(_hwnd);
	hdc.clearBg();

	const auto& sysmetrics = _dm->data();

	int offsetY;
	_vScrollInfo->getPos(offsetY);

	int offsetX;
	_hScrollInfo->getPos(offsetX);

	DefinationRemarks* dm = constCast(_dm);

	int i = 0;
	int outValue;

	for (auto& item : sysmetrics) {
		int x = 0 - offsetX;
		int y = _tm.aveCharHeight * i - offsetY;

		hdc.Fmt_textOut(x, y, "{:03d} {}", i, item.name);

		x += 20 * _tm.aveCharWidthUpperCase;
		hdc.textOut(x, y, item.remarks);

		hdc.setTextAlign(PW5_TextAlignmentOption::Right | PW5_TextAlignmentOption::Top);
		x += 50 * _tm.aveCharWidth;

		dm->invoke(outValue, item, this);
		hdc.Fmt_textOut(x, y, "{:5d}", outValue);

		hdc.setTextAlign(PW5_TextAlignmentOption::Left | PW5_TextAlignmentOption::Top); // reset text align
		++i;
	}
}

}

#endif // SGE_OS_WINDOWS