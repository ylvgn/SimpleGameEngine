#pragma once

namespace sge {

#define Interpolation_ENUM_LIST(E) \
	E(Constant,) \
	E(Linear,) \
	E(Cubic,) \
// ----------
SGE_ENUM_CLASS(Interpolation, u8)

}