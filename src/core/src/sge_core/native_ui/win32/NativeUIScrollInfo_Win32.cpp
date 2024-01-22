#if SGE_OS_WINDOWS

#include "NativeUIScrollInfo_Win32.h"
#include <sge_core/math/Math.h>

namespace sge {

void NativeUIScrollInfo_Win32::onCreate(CreateDesc& desc) {
	_dirty		= false;
	_si			= {}; // ZeroMemory
	_si.cbSize	= sizeof(_si);

	_type = desc.axis == CreateDesc::Axis::Vertical ? SB_VERT : SB_HORZ;
	_step = desc.step;

	setRange(desc.range.min(), desc.range.max());
	setPage(desc.page);
	scrollTo(desc.range.value());
}

void NativeUIScrollInfo_Win32::onScrollTo(int toPos) {
	int pos = rangeMin() + Math::clamp(toPos - rangeMin(), 0, static_cast<int>(range() - page()));
	setPos(pos);
}

void NativeUIScrollInfo_Win32::setRange(int min, int max) {
	if (_si.nMin != min || _si.nMax != max) {
		_dirty = true;
		_si.fMask |= SIF_RANGE;

		_si.nMin = min;
		_si.nMax = max;
	}
}

void NativeUIScrollInfo_Win32::setPage(UINT newPage) {
	// set a proportional scroll bar thumb
	if (_si.nPage != newPage) {
		_dirty = true;
		_si.fMask |= SIF_PAGE;

		_si.nPage = newPage;
	}
}

void NativeUIScrollInfo_Win32::setPos(int newPos) {
	if (_si.nPos != newPos) {
		_dirty = true;
		_si.fMask |= SIF_POS;

		_si.nPos = newPos;
	}
}

void NativeUIScrollInfo_Win32::setTrackPos(int newTrackPos) {
	if (_si.nTrackPos != newTrackPos) {
		_dirty = true;
		_si.fMask |= SIF_TRACKPOS;

		_si.nTrackPos = newTrackPos;
	}
}

void NativeUIScrollInfo_Win32::getRange(HWND hwnd, int& outMin, int& outMax) {
	_retrieve(hwnd, SIF_RANGE);
	outMin = _si.nMin;
	outMax = _si.nMax;
}

void NativeUIScrollInfo_Win32::getPage(HWND hwnd, UINT& out) {
	_retrieve(hwnd, SIF_PAGE);
	out = _si.nPage;
}

void NativeUIScrollInfo_Win32::getPos(HWND hwnd, int& out) {
	_retrieve(hwnd, SIF_POS);
	out = _si.nPos;
}

void NativeUIScrollInfo_Win32::getTrackPos(HWND hwnd, int& out) {
	// only while processing a WM_VSCROLL or WM_HSCROLL message
		// with a notification code of SB_THUMBTRACK or SB_THUMBPOSITION
	_retrieve(hwnd, SIF_TRACKPOS, false);
	out = _si.nTrackPos; // 32-bit thumb position
}

void NativeUIScrollInfo_Win32::reset(HWND hwnd, bool redraw /*= true*/) {
	if (!_dirty)
		return;

	::SetScrollInfo(hwnd, _type, &_si, redraw);
	_dirty = false;
}

SCROLLINFO NativeUIScrollInfo_Win32::toSCROLLINFO(HWND hwnd) {
	_retrieve(hwnd);

	SCROLLINFO res = _si;
	res.fMask = SIF_ALL;
	return res;
}

void NativeUIScrollInfo_Win32::_retrieve(HWND hwnd, int flag /*= SIF_ALL*/, bool redraw /*= true*/) {
	reset(hwnd, redraw);
	ScopedValue<UINT> v(&_si.fMask, flag);
	::GetScrollInfo(hwnd, _type, &_si);
}

}

#endif // SGE_OS_WINDOWS