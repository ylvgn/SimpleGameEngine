#include "EditorContext.h"

namespace sge {

EditorContext* EditorContext::s_instance = nullptr;

EditorContext::EditorContext() {

#define E(T) { \
		static EditorPropertyDrawer_##T drawer; \
		registerPropertyDrawer(TypeOf<T>(), &drawer); \
	} \
// ----------
	E(float)
	E(Vec3f)
	E(Quat4f)
#undef E
}

EditorContext* EditorContext::create() {
	SGE_ASSERT(s_instance == nullptr);
	s_instance = new EditorContext();
	return s_instance;
}

void EditorContext::destroy() {
	if (s_instance != nullptr) {
		s_instance->~EditorContext();
		s_instance = nullptr;
	}
}

void EditorContext::registerPropertyDrawer(const TypeInfo* type, EditorPropertyDrawer* drawer) {
	_propertyDrawerMap[type] = drawer;
}

EditorPropertyDrawer* EditorContext::getPropertyDrawer(const TypeInfo* type) {
	auto it = _propertyDrawerMap.find(type);
	if (it == _propertyDrawerMap.end()) {
		if (type->fields().size()) { // struct
			auto* drawer = &_defaultStructPropertyDrawerMap[type];
			registerPropertyDrawer(type, drawer);
			return drawer;
		}
		return nullptr;
	}
	return it->second;
}

} // namespace