#include "Quat4.h"

namespace sge {

#define Quat4f_FieldInfo_LIST() \
	SGE_FIELD_INFO(x), \
	SGE_FIELD_INFO(y), \
	SGE_FIELD_INFO(z), \
	SGE_FIELD_INFO(w), \
// ------------
SGE_STRUCT_TYPE_IMPL(Quat4f)

#define Quat4d_FieldInfo_LIST() \
	SGE_FIELD_INFO(x), \
	SGE_FIELD_INFO(y), \
	SGE_FIELD_INFO(z), \
	SGE_FIELD_INFO(w), \
// ------------
SGE_STRUCT_TYPE_IMPL(Quat4d)

} // namespace
