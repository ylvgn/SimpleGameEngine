#include "Component.h"

namespace sge {

template<> const TypeInfo* TypeInfo_get<Component>() {
	class Ti : public Component::TI_Base {
	public:
		Ti() {}
	};
	static Ti ti;
	return &ti;
}

} // namespace