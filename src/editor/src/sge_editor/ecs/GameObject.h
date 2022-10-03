#pragma once
#include "TypeObject.h"
#include "Component.h"
#include "Transform.h"

namespace sge {

class GameObject : public TypeObject {
	SGE_TYPEOF_DEFINE(GameObject, TypeObject)
public:
	GameObject();
	~GameObject();

	template<class T> inline
	T* addComponent() {
		static_assert(std::is_base_of<Component, T>::value, "invalid addComponent");
		UPtr<Component> c = UPtr<Component>(new T(this));
		auto& res = _components.emplace_back(std::move(c));
		return sge_cast<T>(res.get());
	}

	Component* addComponent(Component* c) {
		if (!c) return nullptr;
		_components.emplace_back(c);
		return c;
	}

	template<class T> inline
	T* getComponent() {
		static_assert(std::is_base_of<Component, T>::value, "invalid getComponent");
		const auto* target = sge_typeof<T>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				return sge_cast<T>(c.get());
			}
		}
		return nullptr;
	}

	template<class T> inline
	void getComponents(Vector<T*>& out) {
		static_assert(std::is_base_of<Component, T>::value, "invalid getComponents");
		out.reserve(_components.size());
		const auto* target = sge_typeof<T>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				out.emplace_back(sge_cast<T>(c.get()));
			}
		}
	}

	template<class T> inline
	void removeComponent() {
		static_assert(std::is_base_of<Component, T>::value, "invalid removeComponent");
		const auto* target = sge_typeof<T>();
		for (auto* it = _components.begin(); it != _components.end(); ++it) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
				return;
			}
		}
	}

	template<class T> inline
	void removeComponents() {
		static_assert(std::is_base_of<Component, T>::value, "invalid removeComponents");
		const auto* target = sge_typeof<T>();
		for (auto* it = _components.begin(); it != _components.end();) {
			if (it->get()->getType() == target) {
				it = _components.erase(it);
			} else {
				++it;
			}
		}
	}

	Span<UPtr<Component>> components() { return _components; }
private:
	Vector<UPtr<Component>, 2> _components;
};

} // namespace