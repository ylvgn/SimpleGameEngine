#pragma once
#include "MyShape.h"

namespace sge {

const MyTypeInfo* MyShape::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			static MyFieldInfo fi[] = {
				MyFieldInfo("x", &This::x),
				MyFieldInfo("y", &This::y),
			};
			fieldArray = fi;
		}
	};
	static TI ti;
	return &ti;
};

const MyTypeInfo* MyCircle::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			static MyFieldInfo fi[] = {
				MyFieldInfo("r", &This::r),
				MyFieldInfo("testVectorInt", &This::testVectorInt),
				MyFieldInfo("testVectorStr", &This::testVectorStr),
				//MyFieldInfo("testVectorVectorInt", &This::testVectorVectorInt), // not ok
			};
			fieldArray = fi;
		}
	};
	static TI ti;
	return &ti;
}

const MyTypeInfo* MyRect::s_getType() {
	class TI : public TI_Base {
	public:
		TI() {
			static MyFieldInfo fi[] = {
				MyFieldInfo("w", &This::w),
				MyFieldInfo("h", &This::h),
			};
			fieldArray = fi;
		}
	};
	static TI ti;
	return &ti;
}

} // namespace