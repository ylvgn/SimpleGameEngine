#include "CCamera.h"

namespace sge {

const TypeInfo* CCamera::s_getType() {
	class Ti : public CCamera::TI_Base {
	public:
		Ti() {

		}
	};
	static Ti ti;
	return &ti;
}


}