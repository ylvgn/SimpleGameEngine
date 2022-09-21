#include "GameObject.h"

namespace sge {

GameObject::GameObject() {
}

GameObject::~GameObject() {
	_components.clear();
}

SGE_TYPE_IMPL_s_getType(GameObject)

} // namespace