#pragma once

#include <sge_engine/components/Component.h>

namespace sge {

enum class EntityId : u64 { None = 0 };

class Entity : public Object {
	SGE_OBJECT_TYPE(Entity, Object)
public:
	Entity(EntityId id) { _id = id; }

	EntityId id() const			{ return _id; }

	void setName(StrView name)	{ _name = name; }
	StrView name() const		{ return _name; }

	Entity* parent() const		{ return _parent; }
	void setParent(Entity* parent);

	Span<SPtr<Entity>> children() { return _children; }
	void addChild(Entity* child);
	void removeChild(Entity* child);

	Span<SPtr<Component>> components() { return _components; }

	template<class C> inline
	C* addComponent() {
		auto* p = new C();
		p->internal_setEntity(this);
		_components.emplace_back(p);
		return p;
	}

	Component* addComponent(Component* p) {
		if (!p) return nullptr;
		p->internal_setEntity(this);
		_components.emplace_back(p);
		return p;
	}

	template<class C> inline
	C* getComponent() {
		const auto* target = TypeOf<C>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				return sge_cast<C>(c.get());
			}
		}
		return nullptr;
	}

	template<class C> inline
	void getComponents(Vector<C*>& out) {
		out.reserve(_components.size());
		const auto* target = TypeOf<C>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				out.emplace_back(sge_cast<C>(c.get()));
			}
		}
	}

	template<class C> inline
	void removeComponent() {
		const auto* target = TypeOf<C>();
		for (auto* it = _components.begin(); it != _components.end(); ++it) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
				return;
			}
		}
	}

	template<class C> inline
	void removeComponents() {
		const auto* target = TypeOf<C>();
		for (auto* it = _components.begin(); it != _components.end();) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
			} else {
				++it;
			}
		}
	}
private:

	EntityId	_id = EntityId::None;
	String		_name;
	Entity*		_parent = nullptr;

	Vector<SPtr<Component>> _components;
	Vector<SPtr<Entity>> _children;
};

} // namespace