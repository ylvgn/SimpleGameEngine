#pragma once

#if SGE_OS_WINDOWS

#include "../../PW5_NativeUI.h"

namespace sge {

class PW5_MyMappingMode : public PW5_NativeUIWindow {
	using This = PW5_MyMappingMode;
	using Base = PW5_NativeUIWindow;

protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onDraw() override;
private:
	void _example1();
	void _example2();

	void _example3();
	void _drawClientRectCoordinate(PW5_MapMode v);

	void _example4();
	void _example5();
	void _example6();
	void _calcMappingModeExtRatio(PW5_MapMode v);

	void _example7();
	void _drawDeviceCoordinate();
	void _drawLogicalCoordinate();

	void _example8();

	bool _isFirstFrame : 1;
};

}

#endif // SGE_OS_WINDOWS