#pragma once

#include "../Entity.h"

namespace sge {

class Component;
template<> const TypeInfo* TypeInfo_get<Component>();

class Component : public Object {
	SGE_TYPEOF_DEFINE(Component, Object)
public:
	Component(Entity* entity) : _entity(entity) {}

protected:
	Entity* _entity = nullptr;
};

} // namespace