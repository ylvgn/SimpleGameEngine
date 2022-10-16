#include "Entity.h"

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

void Entity::setParent(Entity* parent) {
	if (parent == this) return;
	if (_parent == parent) return;

	if (_parent) {
		_parent->removeChild(this);
	}
	_parent = parent;
	if (parent) {
		parent->addChild(this);
	}
}

void Entity::addChild(Entity* child) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if ((*it)->id() == child->id())
			return;
	}
	_children.emplace_back(SPtr<Entity>(child));
	child->_parent = this;
}

void Entity::removeChild(Entity* child) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if ((*it)->id() == child->id()) {
			//_children.erase(it);
			std::swap(_children.back(), *it);
			child->_parent = nullptr;
			_children.pop_back();
			return;
		}
	}
}

} // namespace