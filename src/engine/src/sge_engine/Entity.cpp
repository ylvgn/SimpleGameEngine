#include "Entity.h"
#include "components/CTransform.h"

namespace sge {

const TypeInfo* Entity::s_getType() {
	class Ti : public Entity::TI_Base {
	public:
		Ti() {

		}
	};
	static Ti ti;
	return &ti;
}

Entity::~Entity() {
}

Entity::Entity() {
	_transform = addComponent<CTransform>();
}

} // namespace