#include "CCamera.h"

namespace sge {

const TypeInfo* CCamera::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
		}
	};
	static TI ti;
	return &ti;
}


}