#include "MyQuat4.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template MyQuat4<float>;
template MyQuat4<double>;

}