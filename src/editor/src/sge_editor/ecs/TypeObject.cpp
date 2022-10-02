#include "TypeObject.h"

namespace sge {

const TypeInfo* TypeObject::s_getType() {
	class Ti : public TI_Base {
	public:
		Ti() {}
	};
	static Ti ti;
	return &ti;
}

} // namespace