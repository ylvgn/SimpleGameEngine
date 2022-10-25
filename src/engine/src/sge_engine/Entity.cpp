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
	_children.clear();
	_components.clear();
}

void Entity::setParent(Entity* parent) {
	if (parent == this) return;
	if (_parent == parent) return;

	if (_parent) {
		_parent->removeChild(this);
	}
	_parent = parent;

	if (!parent) return;

	parent->addChild(this);
	auto* pt = parent->transform();

	auto* t = transform();
	t->setPosition(t->getPosition() - pt->getPosition());
	t->setRotate(t->getRotate() - pt->getRotate());
	t->setScale(t->getScale() / pt->getScale());
}

bool Entity::addChild(Entity* child) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if ((*it)->id() == child->id()) {
			return false;
		}
	}

	_children.emplace_back(child);
	child->_parent = this;
	return true;
}

bool Entity::removeChild(Entity* child) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if ((*it)->id() == child->id()) {
			_children.erase(it); // shoule be ordered
			return true;
		}
	}
	return false;
}

} // namespace