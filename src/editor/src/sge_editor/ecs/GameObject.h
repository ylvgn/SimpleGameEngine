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
		_components.emplace_back(std::move(c));
		return nullptr;
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
	Vector<T*> getComponents() {
		Vector<T*> res;
		res.reserve(_components.size())
		static_assert(std::is_base_of<Component, T>::value, "invalid getComponent");
		const auto* target = sge_typeof<T>();
		for (auto& c : _components) {
			if (c.get()->getType() == target) {
				res.emplace_back(c);
			}
		}
		return res;
	}

	template<class T> inline
	T* removeComponent() {
		static_assert(std::is_base_of<Component, T>::value, "invalid getComponent");
		const auto* target = sge_typeof<T>();
		for (auto& it = _components.begin(); it != _components.end(); it++) {
			if (c.get()->getType() == target) {
				it = _components.erase(it);
				return;
			}
		}
	}

	Span<UPtr<Component>> components() { return _components; }
private:
	Vector<UPtr<Component>, 2> _components;
};

} // namespace