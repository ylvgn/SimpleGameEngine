#pragma once

#if SGE_OS_WINDOWS

#include "PW5_GDI__Object.h"

namespace sge {

class ScopedSelectStockFont : public ScopedSelectStockObject {
	using Base = ScopedSelectStockObject;
public:
	ScopedSelectStockFont(::HDC hdc, PW5_StockLogicalObject_Font flag)
		: Base(hdc, PW5_StockLogicalObject_make(flag)) {}
};

}

#endif