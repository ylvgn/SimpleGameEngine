#include "Component.h"

namespace sge {

const TypeInfo* Component::s_getType() {
	class Ti : public Component::TI_Base {
	public:
		Ti() {
		}
	};
	static Ti ti;
	return &ti;
}

} // namespace