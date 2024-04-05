#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_NativeUI.h"
#include "../PW5_MyDefinationRemarks.h"

namespace sge {

class PW5_MyDefineMarkWindow : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyDefineMarkWindow;
	using DefinationRemarks = PW5_MyDefinationRemarks;
public:
	HDC _hdc = nullptr;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) override;
	virtual void onUIMouseEvent(UIMouseEvent& ev) override;
	virtual void onDraw() override;

	int _cxChar, _cyChar, _cxCaps;

	const DefinationRemarks* _dm = nullptr;
};

PW5_MYDEFINATIONREMARKS(MySysmetricsDM)

}

#endif