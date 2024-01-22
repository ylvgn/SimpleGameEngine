#pragma once

#include "../../PW5_NativeUI.h"

#if SGE_OS_WINDOWS

namespace sge {
/*
	-MessageBox
*/

class PW5_HelloMsg : public PW5_NativeUIWindow {
	using This = PW5_HelloMsg;
	using Base = PW5_NativeUIWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
};

}

#endif