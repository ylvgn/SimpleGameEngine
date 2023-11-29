#pragma once

#include "ProgWin5WindowBase.h"

namespace sge {

class PW5_HelloWin : public ProgWin5WindowBase {
	using This = PW5_HelloWin;
	using Base = ProgWin5WindowBase;
protected:
	virtual void onCreate(CreateDesc& desc) override;
private:
	static LRESULT CALLBACK s_WndProc(HWND, UINT, WPARAM, LPARAM);
};

}