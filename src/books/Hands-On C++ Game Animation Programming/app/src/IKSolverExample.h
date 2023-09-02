#pragma once

#include "IKExampleTestBase.h"

namespace sge {

template<class IKSolver>
class IKSolverExample : public IKExampleTestBase<IKSolver> {
	using Base = typename IKExampleTestBase<IKSolver>;
	using This = typename IKSolverExample<IKSolver>;
public:
	IKSolverExample() : Base() { }
};

}