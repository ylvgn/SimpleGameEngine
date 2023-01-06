#pragma once

#include <sge_engine/components/Component.h>

namespace sge {

class Scene;
class CTransform;

enum class EntityId : u64 { None = 0 };
SGE_FORMATTER_ENUM_AS_INT(EntityId)

class Entity : public Object {
	SGE_OBJECT_TYPE(Entity, Object)
public:
	Entity();

	void setName(StrView name)			{ _name = name; }
	StrView name() const				{ return _name; }

	void		_internalInit(Scene* scene, EntityId id) { _scene = scene; _id = id; }

	Scene*		scene() const { return _scene; }
	EntityId	id() const { return _id; }

	template<class C> inline
	C* addComponent() {
		auto* p = new C();
		p->_internalSetEntity(this);
		_components.emplace_back(p);
		return p;
	}

	template<class C> inline
	C* getComponent() {
		const auto* type = TypeOf<C>();
		for (auto& c : _components) {
			if (c.ptr()->getType() == type) {
				return c;
			}
		}
		return nullptr;
	}

	template<class C> inline
	bool removeComponent() {
		const auto* target = TypeOf<C>();
		for (auto* it = _components.begin(); it != _components.end(); ++it) {
			if (it->get()->getType() == target) {
				_components.erase(it);
				return true;
			}
		}
		return false;
	}

	template<class C> inline
	void getComponents(Vector<C*>& out) {
		out.clean();
		out.reserve(_components.size());
		const auto* target = TypeOf<C>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				out.emplace_back(c.ptr());
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

	Span< SPtr<Component> > components() { return _components; }

	CTransform* transform() { return _transform.ptr(); };

private:
	Vector< SPtr<Component> >	_components;

	String				_name;
	Scene*				_scene		= nullptr;
	EntityId			_id			= EntityId::None;
	SPtr<CTransform>	_transform	= nullptr;
};

} // namespace