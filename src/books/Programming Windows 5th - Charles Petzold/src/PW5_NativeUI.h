#pragma once

#include "PW5_Common.h"
#include "PW5_Win32Util.h"

namespace sge {

class PW5_NativeUIWindow : public NativeUIWindow {
protected:
	virtual void onCloseButton() override;
};

}