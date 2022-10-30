#pragma once

#include "Entity.h"

namespace sge {

class Scene : public NonCopyable {
public:

	Scene();

	Entity* addEntity();
	Entity* addEntity(StrView name);
	Entity* rootEntity() { return _rootEntity.ptr(); }
	Entity* findEntityById(EntityId id);

private:
	u64	_nextEntityId = 0;

	SPtr<Entity> _rootEntity;
	Map<EntityId, SPtr<Entity>> _entityIdMap;
};

} // namespace