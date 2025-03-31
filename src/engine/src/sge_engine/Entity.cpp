#include "Entity.h"
#include "components/CTransform.h"

namespace sge {

const TypeInfo* Entity::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
		}
	};
	static TI ti;
	return &ti;
}

Entity::Entity() {
	_transform = addComponent<CTransform>();
}

} // namespace sge