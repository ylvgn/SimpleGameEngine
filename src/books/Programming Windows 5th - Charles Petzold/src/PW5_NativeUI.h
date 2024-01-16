#pragma once

#include "PW5_Common.h"
#include "PW5_Common_Win32.h"

#include <sge_core/native_ui/NativeUI.h>

namespace sge {

class PW5_NativeUIWindow : public NativeUIWindow {
protected:
	virtual void onCloseButton() override;
};

}