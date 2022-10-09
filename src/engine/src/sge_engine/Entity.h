#pragma once

namespace sge {

class Component;

enum EntityId : u64 { None = 0 };

class Entity;
template<> const TypeInfo* TypeInfo_get<Entity>();

class Entity : public Object {
	SGE_TYPEOF_DEFINE(Entity, Object)
public:
	Entity(EntityId id) { _id = id; }

	void setName(StrView name) { _name = name; }
	StrView name() const { return _name; }

	EntityId	id() const { return _id; }

	template<class C> inline
	C* addComponent() {
		auto* c = new C(this);
		_components.emplace_back(c);
		return c;
	}

	Component* addComponent(Component* c) {
		if (!c) return nullptr;
		_components.emplace_back(c);
		return c;
	}

	template<class C> inline
	C* getComponent() {
		const auto* target = TypeInfo_get<C>();
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
		const auto* target = TypeInfo_get<C>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				out.emplace_back(sge_cast<C>(c.get()));
			}
		}
	}

	template<class C> inline
	void removeComponent() {
		const auto* target = TypeInfo_get<C>();
		for (auto* it = _components.begin(); it != _components.end(); ++it) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
				return;
			}
		}
	}

	template<class C> inline
	void removeComponents() {
		const auto* target = TypeInfo_get<C>();
		for (auto* it = _components.begin(); it != _components.end();) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
			} else {
				++it;
			}
		}
	}

	Span<SPtr<Component>> components() { return _components; }
private:

	String		_name;
	EntityId	_id;

	Vector<SPtr<Component>> _components;
};

} // namespace