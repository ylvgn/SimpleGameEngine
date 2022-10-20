#pragma once

namespace sge {

class Entity;

class Component : public Object {
	SGE_OBJECT_TYPE(Component, Object)
public:
	void internal_setEntity(Entity* e) { _entity = e; }
	virtual ~Component() = default;

	Entity* entity() const { return _entity; }

protected:
	Entity* _entity = nullptr;
};

} // namespace