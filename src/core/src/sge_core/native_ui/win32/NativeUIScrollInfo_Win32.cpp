#if SGE_OS_WINDOWS

#include "NativeUIScrollInfo_Win32.h"
#include "NativeUIWindow_Win32.h"
#include <sge_core/math/Math.h>

namespace sge {

NativeUIScrollInfo_Win32::NativeUIScrollInfo_Win32(NativeUIWindow_Win32* window, CreateDesc& desc)
	: _window(window)
	, _dirty(false)
{
	SGE_ASSERT(window != nullptr);

	_si = {}; // ZeroMemory
	_si.cbSize = sizeof(_si);

	_step		= desc.step;
	_win32_axis = desc.axis == CreateDesc::Axis::Vertical ? SB_VERT : SB_HORZ;

	onSetRange(desc.range.x, desc.range.y);
	onSetPage(desc.page);
}

void NativeUIScrollInfo_Win32::onScrollTo(int toPos) {
	_refreshIfDirty();
	int origin = _win32_rangeMin();
	int offset = Math::clamp(toPos - origin, 0, _maxScrollPos);
	setPos(origin + offset);
}

void NativeUIScrollInfo_Win32::onScrollToHome() {
	int range; onGetRange(range); SGE_UNUSED(range);
	return isVertical() ? _window->scrollWindow({ 0, -_win32_rangeMax() }) : _window->scrollWindow({ -_win32_rangeMax(), 0 });
}

void NativeUIScrollInfo_Win32::onScrollToEnd() {
	int range; onGetRange(range); SGE_UNUSED(range);
	return isVertical() ? _window->scrollWindow({0, _win32_rangeMax()}) : _window->scrollWindow({ _win32_rangeMax(), 0});
}

void NativeUIScrollInfo_Win32::onScrollStepForward () {
	return isVertical() ? _window->scrollWindow({0, _step}) : _window->scrollWindow({ _step, 0 });
}

void NativeUIScrollInfo_Win32::onScrollStepBackward() {
	return isVertical() ? _window->scrollWindow({ 0, -_step }) : _window->scrollWindow({ -_step, 0 });
}

void NativeUIScrollInfo_Win32::onScrollPageForward () {
	int page; onGetPage(page);
	return isVertical() ? _window->scrollWindow({ 0, page }) : _window->scrollWindow({ page, 0 });
}
void NativeUIScrollInfo_Win32::onScrollPageBackward() {
	int page; onGetPage(page);
	return isVertical() ? _window->scrollWindow({ 0, -page }) : _window->scrollWindow({ -page, 0 });
}

void NativeUIScrollInfo_Win32::onSetPos(int newPos) {
	if (_si.nPos != newPos) {
		_dirty = true;
		_si.fMask |= SIF_POS;

		_si.nPos = newPos;
	}
}

void NativeUIScrollInfo_Win32::onSetStep(int newStep) {
	_step = newStep;
}

void NativeUIScrollInfo_Win32::onSetRange(int min, int max) {
	if (_si.nMin != min || _si.nMax != max) {
		_dirty = true;
		_si.fMask |= SIF_RANGE;

		_si.nMin = min;
		_si.nMax = max;
	}
}

void NativeUIScrollInfo_Win32::onSetPage(int newPage_) {
	// set a proportional scroll bar thumb
	UINT newPage = Math::max(0, newPage_);
	if (_si.nPage != newPage) {
		_dirty = true;
		_si.fMask |= SIF_PAGE;

		_si.nPage = newPage;
	}
}

void NativeUIScrollInfo_Win32::onGetPos(int& out) {
	_retrieve(SIF_POS);
	out = _si.nPos;
}

void NativeUIScrollInfo_Win32::onGetStep(int& o) {
	o = _step;
}

void NativeUIScrollInfo_Win32::onGetRange(int& o) {
	_retrieve(SIF_RANGE);
	o = Math::max(0, _si.nMax - _si.nMin);
}

void NativeUIScrollInfo_Win32::onGetPage(int& out) {
	_retrieve(SIF_PAGE);
	out = static_cast<int>(_si.nPage);
}

void NativeUIScrollInfo_Win32::onSetTrackPos(int newTrackPos) {
	if (_si.nTrackPos != newTrackPos) {
		_dirty = true;
		_si.fMask |= SIF_TRACKPOS;

		_si.nTrackPos = newTrackPos;
	}
}

void NativeUIScrollInfo_Win32::onGetTrackPos(int& o) {
	// only while processing a WM_VSCROLL or WM_HSCROLL message
		// with a notification code of SB_THUMBTRACK or SB_THUMBPOSITION
	_retrieve(SIF_TRACKPOS, false);
	o = _si.nTrackPos; // 32-bit thumb position
}

void NativeUIScrollInfo_Win32::onGetMaxScrollPos(int& o) {
	_retrieve(SIF_RANGE | SIF_PAGE);
	_refreshScrollMaxPos();
	o = _maxScrollPos;
}

void NativeUIScrollInfo_Win32::refresh(bool redraw /*= true*/) {
	if (!_dirty)
		return;

	::SetScrollInfo(_window->_hwnd, _win32_axis, &_si, redraw);
}

void NativeUIScrollInfo_Win32::_refreshIfDirty() {
	if (!_dirty)
		return;
	
	_retrieve();
	_refreshScrollMaxPos();
	refresh();
	_dirty = false;
}

void NativeUIScrollInfo_Win32::_refreshScrollMaxPos() {
	_maxScrollPos = _win32_range() - _win32_page();
}

void NativeUIScrollInfo_Win32::_retrieve(int flag /*= SIF_ALL*/, bool redraw /*= true*/) {
	refresh(redraw);
	ScopedValue<UINT> v(_si.fMask, flag);
	::GetScrollInfo(_window->_hwnd, _win32_axis, &_si);
}

} // namespace sge

#endif // SGE_OS_WINDOWS