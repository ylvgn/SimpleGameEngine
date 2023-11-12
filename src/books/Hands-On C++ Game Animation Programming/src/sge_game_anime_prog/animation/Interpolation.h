#pragma once

namespace sge {

enum class Interpolation {
	Constant, // step curve
	Linear,
	Cubic,
};

inline
const char* enumStr(Interpolation v) {
	switch (v)
	{
#define E(T) case Interpolation::T: return #T;
		E(Constant)
		E(Linear)
		E(Cubic)
#undef E
	default: SGE_ASSERT(false); return "";
	}
}
SGE_FORMATTER_ENUM(Interpolation)

}