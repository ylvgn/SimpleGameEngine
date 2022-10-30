#pragma once

namespace sge {

class EntitySelection : public NonCopyable {
public:
	void add(EntityId id);
	void select(EntityId id);
	void deselect(EntityId id);
	void deselectAll();
	bool has(EntityId id) const;
	bool isSelected() const { return _entities.size() > 0; }

	Span<const EntityId> entities() const { return _entities; }

private:
	Vector<EntityId>	_entities;
	Set<EntityId>		_entitiesSet;

};

}