#include "Component.h"

namespace sge {

SGE_TYPE_IMPL_s_getType(Component)

Component::Component(GameObject* owner)
	: _entity(owner)
{
}

Component::~Component() {
	_entity = nullptr;
}


} // namespace