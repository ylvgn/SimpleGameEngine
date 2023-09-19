#pragma once

#include "IKExampleTestBase.h"

namespace sge {
/*
	Hinge constraints are often used for elbow or knee joints.
*/
template<class IKSolver>
struct HingeSocketConstraintExample : public IKExampleTestBase<IKSolver> {
	using Base = typename IKExampleTestBase<IKSolver>;
	using This = typename HingeSocketConstraintExample<IKSolver>;
public:
	HingeSocketConstraintExample() : Base() {}

	static HingeSocketConstraintExample* instance();

protected:
	virtual void onCreate() override;

private:
	static void s_constraintHandler(int i, IKSolver* solver);

	Map<int, vec3f> _jointId2LimitAxis;
};


}