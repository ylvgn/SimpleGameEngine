#pragma once

#include "ProgWin5WindowBase.h"

namespace sge {

class PW5_HelloMsg : public ProgWin5WindowBase {
	using This = PW5_HelloMsg;
	using Base = ProgWin5WindowBase;
protected:
	virtual void onCreate(CreateDesc& desc) override;
};

}