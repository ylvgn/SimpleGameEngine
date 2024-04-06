#pragma once

namespace sge {

class PW5_NativeUIWindow : public NativeUIWindow {
protected:
	virtual void onCloseButton() override;
};

}