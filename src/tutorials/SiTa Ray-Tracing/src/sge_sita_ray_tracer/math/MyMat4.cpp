#include "MyMat4.h"

namespace sge {

// explicit specialization to force VisualC check syntax in function body

template MyMat4<float>;
template MyMat4<double>;

}