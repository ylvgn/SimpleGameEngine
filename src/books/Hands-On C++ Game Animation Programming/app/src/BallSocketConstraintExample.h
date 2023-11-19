#pragma once

#include "IKExampleTestBase.h"

namespace sge {
/*
	The ball-and-socket constraint is usually applied to the hip or shoulder joints of a character.
	These also tend to be the root joints for the limb IK chains.
*/

template<class IKSolver>
class BallSocketConstraintExample : public IKExampleTestBase<IKSolver> {
	using Base = typename IKExampleTestBase<IKSolver>;
	using This = typename BallSocketConstraintExample<IKSolver>;
public:
	BallSocketConstraintExample() : Base() {}

	static BallSocketConstraintExample* instance();

protected:
	virtual void onCreate(Request& req) override;

private:
	static void s_constraintHandler(int i, IKSolver* solver);

	Map<int, float> _jointId2LimitAngle;
};

}