#pragma once

#include "Entity.h"

namespace sge {

class Scene : public NonCopyable {
public:

	~Scene() { destroy(); }

	Entity* addEntity();
	Entity* addEntity(StrView name);

	Span<Entity*> entities() { return _entities; }

	Entity* findEntityById(EntityId id);

	void destroy();

private:
	u64	_nextEntityId = 0;

	Vector<Entity*> _entities;
	Map<EntityId, SPtr<Entity>> _entityIdMap;
};

} // namespace