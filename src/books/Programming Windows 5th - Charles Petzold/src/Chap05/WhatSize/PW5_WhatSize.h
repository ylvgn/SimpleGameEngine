#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_WhatSize : public PW5_NativeUIWindow {
	using This		= PW5_WhatSize;
	using Base		= PW5_NativeUIWindow;
	using MapMode	= PW5_MapMode;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
private:
	void _show(MapMode v);

	bool			_isFirstFrame : 1;

	MyTextMetrics	_tm;
	Vec2i			_framePos;
};

}

#endif // SGE_OS_WINDOWS