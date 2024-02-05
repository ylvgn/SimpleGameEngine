#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_NativeUI.h"

namespace sge {

SGE_DEFINE_MARK_STRUCT(MySysmetricsDM)

class PW5_MyDefineMarkInfoWindow : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyDefineMarkInfoWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) override;
	virtual void onDraw() override;

	const DefineMarkInfo* _dmInfo = nullptr;

private:
	int _cxChar, _cyChar, _cxCaps;
};

}

#endif