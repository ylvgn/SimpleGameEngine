#pragma once

#if SGE_OS_WINDOWS

#include "PW5_GDI__HDC.h"

namespace sge {

class ScopedSelectObject : public MyHDC_Base {
public:
	ScopedSelectObject(::HDC hdc, const ::HGDIOBJ& obj) {
		_hdc = hdc;
		_last = ::SelectObject(hdc, obj);
	}
	virtual ~ScopedSelectObject() {
		::SelectObject(_hdc, _last);
		_hdc	= nullptr;
		_last	= nullptr;
	}
private:
	::HGDIOBJ _last = nullptr;
};

class ScopedSelectStockObject : public ScopedSelectObject {
	using Base = ScopedSelectObject;
public:
	ScopedSelectStockObject(::HDC hdc, StockObj flag)
		: Base(hdc, GDI::getStockObject(flag)) {}
};

}

#endif