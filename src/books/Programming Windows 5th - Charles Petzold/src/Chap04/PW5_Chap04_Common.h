#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_NativeUI.h"

namespace sge {

class PW5_MyDefineMarkWindow : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyDefineMarkWindow;
public:
	HDC _hdc = nullptr;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onDraw() override;

	int _cxChar, _cyChar, _cxCaps;

	const DefineMark* _dm = nullptr;
};

SGE_DEFINEMARK(MySysmetricsDM)

}

#endif