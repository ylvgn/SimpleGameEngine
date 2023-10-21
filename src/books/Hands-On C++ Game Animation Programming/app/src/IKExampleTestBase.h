#pragma once

#include "ExampleTestBase.h"

#include <sge_game_anime_prog/math/Transform.h>
#include <sge_game_anime_prog/animation/Track.h>
#include <sge_game_anime_prog/animation/TransformTrack.h>

namespace sge {

class DebugDraw_PointLines;

template<class IKSolver>
class IKExampleTestBase : public ExampleTestBase {
	using Base			= ExampleTestBase;
	using SampleRequest = Track_SampleRequest;
	using DebugDrawPL	= DebugDraw_PointLines;
public:
	IKExampleTestBase();

protected:
	virtual void onCreate(Request& req) override;
	virtual void onUpdate(Request& req) override;
	virtual void onRender(Request& req) override;

protected:
	void _createIKChains();
	void _createLoopingClip();

	SPtr<DebugDrawPL>			_ikChainsVisual;
	Vector<UPtr<DebugDraw>, 3>	_targetVisual;

	SampleRequest				_sr;

	Transform					_target;
	TransformTrack				_targetPath;

	UPtr<IKSolver>				_solver;
};

}