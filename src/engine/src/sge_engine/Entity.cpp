#include "Entity.h"

namespace sge {

template<> const TypeInfo* TypeInfo_get<Entity>() {
	class Ti : public Entity::TI_Base {
	public:
		Ti() {}
	};
	static Ti ti;
	return &ti;
}

} // namespace