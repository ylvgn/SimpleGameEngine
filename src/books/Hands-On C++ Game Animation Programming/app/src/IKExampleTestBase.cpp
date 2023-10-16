#include "IKExampleTestBase.h"

namespace sge {

template<class IKSolver>
void IKExampleTestBase<IKSolver>::_createIKChains() {
	constexpr const size_t kJointCount = 6;

	_solver = eastl::make_unique<IKSolver>();
	_solver->resize(kJointCount);

	_solver->setLocalTransform(0, Transform(vec3f::s_zero(), quat4f::s_angleAxis(Math::radians(90.f), vec3f::s_right()), vec3f::s_one()));
	_solver->setLocalTransform(1, Transform(vec3f(0, 0, 1.0f), quat4f::s_identity(), vec3f::s_one()));
	_solver->setLocalTransform(2, Transform(vec3f(0, 0, 1.5f), quat4f::s_identity(), vec3f::s_one()));
	_solver->setLocalTransform(3, Transform(vec3f(0, 0, 0.5f), quat4f::s_angleAxis(Math::radians(90.0f), vec3f::s_up()), vec3f::s_one()));
	_solver->setLocalTransform(4, Transform(vec3f(0, 0, 0.5f), quat4f::s_identity(), vec3f::s_one()));
	_solver->setLocalTransform(5, Transform(vec3f(0, 0, 0.5f), quat4f::s_identity(), vec3f::s_one()));
}

template<class IKSolver>
void IKExampleTestBase<IKSolver>::_createLoopingClip() {
	static constexpr const size_t kFrameCount = 14;
	static constexpr const float  kFactor = 0.5f;

	_sr.time	= 0.0f;
	_sr.isLoop	= true;

	vec3f startPos = vec3f(1,-2,0);
	_target.position = startPos;

	VectorTrack& posTrack = constCast(_targetPath.position());
	posTrack.resize(kFrameCount);
	posTrack[0 ] = FrameUtil::createFrame(0.f,   startPos       * kFactor);
	posTrack[1 ] = FrameUtil::createFrame(1.0f,  vec3f(1, 2, 0) * kFactor);
	posTrack[2 ] = FrameUtil::createFrame(2.0f,  vec3f(1, 4, 0) * kFactor);
	posTrack[3 ] = FrameUtil::createFrame(3.0f,  vec3f(3, 4, 0) * kFactor);
	posTrack[4 ] = FrameUtil::createFrame(4.0f,  vec3f(5, 4, 0) * kFactor);
	posTrack[5 ] = FrameUtil::createFrame(5.0f,  vec3f(5, 4, 2) * kFactor);
	posTrack[6 ] = FrameUtil::createFrame(6.0f,  vec3f(5, 4, 4) * kFactor);
	posTrack[7 ] = FrameUtil::createFrame(7.0f,  vec3f(3, 4, 4) * kFactor);
	posTrack[8 ] = FrameUtil::createFrame(8.0f,  vec3f(3, 2, 4) * kFactor);
	posTrack[9 ] = FrameUtil::createFrame(9.0f,  vec3f(3, 2, 2) * kFactor);
	posTrack[10] = FrameUtil::createFrame(10.0f, vec3f(1, 2, 2) * kFactor);
	posTrack[11] = FrameUtil::createFrame(11.0f, vec3f(1, 0, 2) * kFactor);
	posTrack[12] = FrameUtil::createFrame(12.0f, vec3f(1,-2, 2) * kFactor);
	posTrack[13] = FrameUtil::createFrame(13.0f, startPos       * kFactor);
}

template<class IKSolver>
IKExampleTestBase<IKSolver>::IKExampleTestBase() {
	_createIKChains();
	_createLoopingClip();
}

template<class IKSolver>
void IKExampleTestBase<IKSolver>::onCreate(Request& req) {
	// debugDraw depends on opengl, create it after "gladLoadGL" called
	_ikChainsVisual = new DebugDrawPL();
	_ikChainsVisual->setColor(DebugDraw::kPurple);

	_targetVisual.resize(3);
	for (int i = 0; i < 3; i++) {
		_targetVisual[i] = eastl::make_unique<DebugDraw>(2);
	}
}

template<class IKSolver>
void IKExampleTestBase<IKSolver>::onUpdate(Request& req) {
	_sr.time += req.dt;
	if (_sr.time > _targetPath.getEndTime()) {
		_sr.time -= _targetPath.getEndTime();
	}

	_target = _targetPath.sample(_target, _sr);
	_solver->solve(_target);
}

template<class IKSolver>
void IKExampleTestBase<IKSolver>::onRender(Request& req) {
	mat4f proj(req.camera.projMatrix());
	mat4f view(req.camera.viewMatrix());

	mat4f mvp = proj * view * mat4f::s_identity();

	{ // draw ikChains
		_ikChainsVisual->fromIKSolver(*_solver);
		_ikChainsVisual->draw(mvp);
	}

	{ // 6 DOF(6 points, 3 lines) - xyz axis of target
		(*_targetVisual[0])[0] = _target.position + (vec3f::s_right()   *  kGizmoSize);	//  x red
		(*_targetVisual[0])[1] = _target.position + (vec3f::s_right()   * -kGizmoSize);	// -x red
		(*_targetVisual[1])[0] = _target.position + (vec3f::s_up()      *  kGizmoSize);	//  y green
		(*_targetVisual[1])[1] = _target.position + (vec3f::s_up()      * -kGizmoSize);	// -y green
		(*_targetVisual[2])[0] = _target.position + (vec3f::s_forward() *  kGizmoSize);	//  z blue
		(*_targetVisual[2])[1] = _target.position + (vec3f::s_forward() * -kGizmoSize);	// -z blue

		_targetVisual[0]->uploadToGpu();
		_targetVisual[1]->uploadToGpu();
		_targetVisual[2]->uploadToGpu();

		_targetVisual[0]->draw(DebugDrawMode::Lines, mvp, DebugDraw::kRed);
		_targetVisual[1]->draw(DebugDrawMode::Lines, mvp, DebugDraw::kGreen);
		_targetVisual[2]->draw(DebugDrawMode::Lines, mvp, DebugDraw::kBlue);
	}
}

}