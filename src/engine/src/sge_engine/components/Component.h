#pragma once

namespace sge {

class Entity;

class Component : public Object {
	SGE_OBJECT_TYPE(Component, Object)
public:
	void _internalSetEntity(Entity* e) { _entity = e; }
	virtual ~Component() = default;

	Entity* entity() const { return _entity; }

	template<class C> inline C* getComponent() { SGE_ASSERT(_entity != nullptr); return _entity->getComponent<C>(); }

protected:
	Entity* _entity = nullptr;
};

} // namespace