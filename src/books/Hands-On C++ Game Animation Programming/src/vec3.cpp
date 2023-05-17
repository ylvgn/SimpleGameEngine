#include "vec3.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template TVec3<float>;
template TVec3<int>;
template TVec3<unsigned int>;

}