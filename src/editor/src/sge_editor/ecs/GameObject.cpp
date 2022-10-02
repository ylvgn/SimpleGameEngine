#include "GameObject.h"

namespace sge {

GameObject::GameObject() {
}

GameObject::~GameObject() {
	_components.clear();
}

const TypeInfo* GameObject::s_getType() {
	class Ti : public TI_Base {
	public:
		Ti() {
#if 0
			static FieldInfo fi[] = {
				FieldInfo("_components",	&This::_components),
			};
			setFieldInfo(fi);
#endif
		}
	};
	static Ti ti;
	return &ti;
}

} // namespace