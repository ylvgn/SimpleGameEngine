#include "MyLinearTransform.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template MyLinearTransform<float>;
template MyLinearTransform<double>;

}