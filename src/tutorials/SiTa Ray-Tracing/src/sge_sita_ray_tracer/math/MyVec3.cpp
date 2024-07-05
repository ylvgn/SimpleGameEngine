#include "MyVec3.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template MyVec3<float>;
template MyVec3<double>;

}