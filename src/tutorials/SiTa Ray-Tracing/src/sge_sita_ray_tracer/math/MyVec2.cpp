#include "MyVec2.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template MyVec2<float>;
template MyVec2<double>;

}
