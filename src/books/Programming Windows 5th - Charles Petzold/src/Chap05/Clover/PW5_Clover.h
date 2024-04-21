#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_Clover : public PW5_NativeUIWindow {
	using This	= PW5_Clover;
	using Base	= PW5_NativeUIWindow;
public:
	~PW5_Clover() { _destroy(); }
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
private:
	void _calcClippingRegion();
	void _destroy();

	float	_clientRecthypotLen;

	::HRGN	_lr = nullptr;
	::HRGN	_tb = nullptr;
	::HRGN	_rgn = nullptr;
	::HRGN	_rgnRe = nullptr;
};

}

#endif // SGE_OS_WINDOWS