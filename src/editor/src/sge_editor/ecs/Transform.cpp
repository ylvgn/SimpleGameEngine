#include "Transform.h"

namespace sge {

const TypeInfo* Transform::s_getType() {
	class Ti : public TI_Base {
	public:
		Ti() {
			static FieldInfo fi[] = {
				FieldInfo("localPosition",	&This::localPosition),
				FieldInfo("localScale",		&This::localScale),
				FieldInfo("localRotation",	&This::localRotation),
			};
			fieldArray = fi;
		}
	};
	static Ti ti;
	return &ti;
}

} // namespace