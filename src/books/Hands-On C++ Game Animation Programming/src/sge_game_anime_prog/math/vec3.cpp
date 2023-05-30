#include "vec3.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template TVec3<float>;
//template TVec3<double>; some bug in Math::lerp

}