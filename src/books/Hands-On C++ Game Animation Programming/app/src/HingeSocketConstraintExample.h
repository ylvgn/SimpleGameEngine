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
	using DebugDrawPL = DebugDraw_PointLines;
public:
	HingeSocketConstraintExample() : Base() {}

	static HingeSocketConstraintExample* instance();

protected:
	virtual void onCreate(Request& req) override;
	virtual void onRender(Request& req) override;
private:
	static void s_constraintHandler(int i, IKSolver* solver);

	Map<int, vec3f>				_jointId2LimitAxis;

	Vector< UPtr<DebugDrawPL> >	_limitAxisVisuals;
	Vector< UPtr<DebugDrawPL> >	_jointAxisVisuals;
};

}