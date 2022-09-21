#pragma once
#include "TypeObject.h"

namespace sge {

class GameObject;

class Component : public TypeObject {
	SGE_TYPEOF_DEFINE(Component, TypeObject)
public:
	Component(GameObject* owner);
	~Component();
private:
	const GameObject* _entity = nullptr;
};

} // namespace