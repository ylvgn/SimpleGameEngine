#pragma once

#include "../PW5_Chap04_Common.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MySysMets3 : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MySysMets3;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onShow() override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) override;
	virtual void onDraw() override;
private:
	int _cxChar, _cyChar, _cxCaps;
};

}

#endif // SGE_OS_WINDOWS