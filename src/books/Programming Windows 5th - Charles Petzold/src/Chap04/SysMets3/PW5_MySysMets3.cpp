#if SGE_OS_WINDOWS

#include "PW5_MySysMets3.h"

namespace sge {

void PW5_MySysMets3::onCreate(CreateDesc& desc) {
	_dm = MySysmetricsDM::s_getRemarks();
	Base::onCreate(desc);
}

}

#endif // SGE_OS_WINDOWS