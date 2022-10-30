#include "Scene.h"
#include "Entity.h"
#include "components/CTransform.h"

namespace sge {

Scene::Scene() {
	_rootEntity = addEntity();
}

Entity* Scene::addEntity() {
	auto* p = new Entity();
	p->_internalInit(this, static_cast<EntityId>(++_nextEntityId));
	_entityIdMap[p->id()] = p;

	if (_rootEntity) {
		_rootEntity->transform()->addChild(p->transform());
	}

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