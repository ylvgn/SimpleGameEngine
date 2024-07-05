#include "MyVec4.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template MyVec4<float>;
template MyVec4<double>;

}