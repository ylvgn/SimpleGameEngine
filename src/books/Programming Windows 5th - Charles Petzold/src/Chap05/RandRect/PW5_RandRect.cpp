#if SGE_OS_WINDOWS

#include "PW5_RandRect.h"

namespace sge {

void PW5_RandRect::onCreate(CreateDesc& desc) {
	desc.ownDC = true;
	Base::onCreate(desc);
}

void PW5_RandRect::onUpdate(float dt) {
	ScopedGetDC hdc(_hwnd);

	int w = static_cast<int>(_clientRect.w);
	int h = static_cast<int>(_clientRect.h);

	_rc.setByLTRB(
		static_cast<float>(rand() % w),
		static_cast<float>(rand() % h),
		static_cast<float>(rand() % w),
		static_cast<float>(rand() % h)
	);

	_color.set(
		rand() % 256,
		rand() % 256,
		rand() % 256,
		rand() % 256
	);

	GDI::fillRect(hdc, _rc, _color);
}

}

#endif // SGE_OS_WINDOWS