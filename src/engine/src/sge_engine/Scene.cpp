#include "Scene.h"

namespace sge {

Entity* Scene::addEntity() {
	auto* p = new Entity(static_cast<EntityId>(++_nextEntityId));
	_entityIdMap[p->id()] = p;
	_entities.emplace_back(p);
	return p;
}

Entity* Scene::addEntity(StrView name) {
	auto* p = addEntity();
	p->setName(name);
	return p;
}

Entity* Scene::findEntityById(EntityId id) {
	auto it = _entityIdMap.find(id);
	if (it == _entityIdMap.end()) {
		return nullptr;
	}
	return it->second;
}

} // namespace