#include "HingeSocketConstraintExample.h"

#include <sge_game_anime_prog/ik/CCDSolver.h>
#include <sge_game_anime_prog/ik/FABRIKSolver.h>

namespace sge {

template<class IKSolver>
HingeSocketConstraintExample<IKSolver>* HingeSocketConstraintExample<IKSolver>::instance() {
	static This s;
	return &s;
}

template<class IKSolver>
void HingeSocketConstraintExample<IKSolver>::onCreate(Request& req) {
	Base::onCreate(req);
	static const vec3f sLimitAxis = vec3f::s_up();

	size_t jointCount = _solver->getJointCount();

	_limitAxisVisuals.resize(jointCount);
	_jointAxisVisuals.resize(jointCount * 3);
	for (int i = 3; i < jointCount; ++i) {
		_jointId2LimitAxis[i] = sLimitAxis;

		_limitAxisVisuals[i] = eastl::make_unique<DebugDrawPL>();
		_limitAxisVisuals[i]->setColor(DebugDraw::kYellow);

		int j = i * 3;
		_jointAxisVisuals[j  ] = eastl::make_unique<DebugDrawPL>();
		_jointAxisVisuals[j  ]->setColor(DebugDraw::kRed);
		_jointAxisVisuals[j+1] = eastl::make_unique<DebugDrawPL>();
		_jointAxisVisuals[j+1]->setColor(DebugDraw::kGreen);
		_jointAxisVisuals[j+2] = eastl::make_unique<DebugDrawPL>();
		_jointAxisVisuals[j+2]->setColor(DebugDraw::kBlue);
	}

	_solver->setConstraintsHandler(&s_constraintHandler);
}

template<class IKSolver>
void HingeSocketConstraintExample<IKSolver>::onRender(Request& req) {
	Base::onRender(req);

	mat4f projection(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());
	mat4f mvp = projection * view;

	size_t jointCount = _solver->getJointCount();
	for (int i = 0; i < jointCount; ++i) {
		if (_limitAxisVisuals[i] != nullptr) {
			_limitAxisVisuals[i]->draw(mvp);
		}

		int j = i * 3;
		if (_jointAxisVisuals[j] != nullptr) {
			_jointAxisVisuals[j  ]->draw(mvp, DebugDrawPL::Mask::Line);
			_jointAxisVisuals[j+1]->draw(mvp, DebugDrawPL::Mask::Line);
			_jointAxisVisuals[j+2]->draw(mvp, DebugDrawPL::Mask::Line);
		}
	}
}

template<class IKSolver>
void HingeSocketConstraintExample<IKSolver>::s_constraintHandler(int i, IKSolver* solver) {
	if (solver == nullptr) return;

	auto* exampleThis = HingeSocketConstraintExample<IKSolver>::instance();
	auto& map = exampleThis->_jointId2LimitAxis;

	if (map.find(i) == map.end()) return;
	const vec3f& axis = map[i];

	auto& localTrans = solver->getLocalTransform(i);
	Transform worldTrans = solver->getWorldTransform(i);

	Transform parentTrans = i > 0
		? solver->getWorldTransform(i-1)
		: worldTrans;

	vec3f  currentHinge = worldTrans.rotation * axis;
	vec3f  desiredHinge = parentTrans.rotation * axis;
	quat4f currentToDesired = quat4f::s_fromTo(currentHinge, desiredHinge);

	localTrans.rotation = localTrans.rotation * currentToDesired;

// debug drawline start ------------------------
	using TestBase = ExampleTestBase;
	const vec3f& worldPos = worldTrans.position;

	auto& limitAxisVisual = exampleThis->_limitAxisVisuals[i];
	limitAxisVisual->clear();
	limitAxisVisual->add(worldPos - axis * TestBase::kGizmoSize, worldPos + axis * TestBase::kGizmoSize);

	auto& jointAxisVisuals = exampleThis->_jointAxisVisuals;
	static const vec3f kRight   = vec3f::s_right()   * TestBase::kGizmoSize;
	static const vec3f kUp      = vec3f::s_up()	     * TestBase::kGizmoSize;
	static const vec3f kForward = vec3f::s_forward() * TestBase::kGizmoSize;

	int j = i * 3;
	jointAxisVisuals[j+0]->clear();
	jointAxisVisuals[j+1]->clear();
	jointAxisVisuals[j+2]->clear();

	jointAxisVisuals[j+0]->add(worldPos - kRight,   worldPos + kRight);
	jointAxisVisuals[j+1]->add(worldPos - kUp,      worldPos + kUp);
	jointAxisVisuals[j+2]->add(worldPos - kForward, worldPos + kForward);
// debug drawline end ------------------------
}

template HingeSocketConstraintExample<CCDSolver>;
template HingeSocketConstraintExample<FABRIKSolver>;

}
