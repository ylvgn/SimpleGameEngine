#pragma once

#include <sge_engine/components/Component.h>

namespace sge {

enum class EntityId : u64 { None = 0 };

class CTransform;

class Entity : public Object {
	SGE_OBJECT_TYPE(Entity, Object)
public:
	Entity() = default;
	~Entity();

	void setId(EntityId id)				{ _id = id; }
	EntityId id() const					{ return _id; }

	void setName(StrView name)			{ _name = name; }
	StrView name() const				{ return _name; }

	Entity* parent() const				{ return _parent; }
	void setParent(Entity* parent);

	Span<SPtr<Entity>> children()		{ return _children; }

	Span<SPtr<Component>> components()	{ return _components; }

	void addChild(Entity* child);
	void removeChild(Entity* child);

	CTransform* transform();

	template<class C> inline
	C* addComponent() {
		auto* p = new C();
		p->internal_setEntity(this);

		if (std::is_base_of<CRenderer, C>::value) {
			RendererSystem::instance()->addComponent(reinterpret_cast<CRenderer*>(p));
			return p;
		}

		_components.emplace_back(p);
		return p;
	}

	template<class C> inline
	C* getComponent() {
		if (std::is_base_of<CRenderer, C>::value) {
			return sge_cast<C>(RendererSystem::instance()->getComponent(this));
		}
		const auto* target = TypeOf<C>();
		for (auto& c : _components) {
			if (c.ptr()->getType() == target) {
				return sge_cast<C>(c.ptr());
			}
		}
		return nullptr;
	}

	template<class C> inline
	void removeComponent() {
		if (std::is_base_of<CRenderer, C>::value) {
			RendererSystem::instance()->removeComponent(this);
			return;
		}

		const auto* target = TypeOf<C>();
		for (auto* it = _components.begin(); it != _components.end(); ++it) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
				return;
			}
		}
	}

	// todo
	template<class C> inline
		void getComponents(Vector<C*>& out) {
		out.reserve(_components.size());
		const auto* target = TypeOf<C>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				out.emplace_back(sge_cast<C>(c.ptr()));
			}
		}
	}

	// todo
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
	CTransform* _transform = nullptr;

	Vector<SPtr<Component>> _components;
	Vector<SPtr<Entity>> _children;
};

} // namespace