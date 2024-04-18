#pragma once

#include "PW5_Common.h"

namespace sge {

class PW5_NativeUIWindow : public NativeUIWindow {
public:
	void update(float dt) { onUpdate(dt); }
protected:
	virtual void onCloseButton() override;
	virtual void onUpdate(float dt) {}
};

}