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
	if (parent) {
		parent->addChild(this);

		auto* t = transform();
		t->setPosition(t->getPosition() - _parent->transform()->getPosition());
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

CTransform* Entity::transform() {
	if (!_transform) {
		_transform = getComponent<CTransform>();
	}
	SGE_ASSERT(_transform != nullptr);
	return _transform;
}

} // namespace