#pragma once

#include "../../PW5_NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {

class PW5_MyMessageBox : public PW5_NativeUIWindow {
	using This = PW5_MyMessageBox;
	using Base = PW5_NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
};

}

#endif // SGE_OS_WINDOWS