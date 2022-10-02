#include "Component.h"

namespace sge {

Component::Component(GameObject* owner)
	: _entity(owner)
{
}

Component::~Component() {
	_entity = nullptr;
}

const TypeInfo* Component::s_getType() {
	class Ti : public TI_Base {
	public:
		Ti() {}
	};
	static Ti ti;
	return &ti;
}

} // namespace