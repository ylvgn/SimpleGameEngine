#include "Vec3.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

#ifndef SGE_MATH_USE_SSE
	#error
#elif SGE_MATH_USE_SSE
	template Vec3_SSE<float>;
	template Vec3_SSE<double>;

#else
	template Vec3_Basic<float>;
	template Vec3_Basic<double>;
#endif


#define Vec3f_FieldInfo_LIST() \
	SGE_FIELD_INFO(x), \
	SGE_FIELD_INFO(y), \
	SGE_FIELD_INFO(z), \
// ------------
SGE_STRUCT_TYPE_IMPL(Vec3f)

#define Vec3d_FieldInfo_LIST() \
	SGE_FIELD_INFO(x), \
	SGE_FIELD_INFO(y), \
	SGE_FIELD_INFO(z), \
// ------------
SGE_STRUCT_TYPE_IMPL(Vec3d)

}