#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

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