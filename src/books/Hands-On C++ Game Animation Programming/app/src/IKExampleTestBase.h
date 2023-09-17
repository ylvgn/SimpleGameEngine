#pragma once

#include "ExampleTestBase.h"

#include <sge_game_anime_prog/math/Transform.h>
#include <sge_game_anime_prog/animation/Track.h>
#include <sge_game_anime_prog/animation/TransformTrack.h>

namespace sge {

class DebugDraw_PointLines;

template<class IKSolver>
class IKExampleTestBase : public ExampleTestBase {
	using Base = ExampleTestBase;
	using DebugDrawPL = DebugDraw_PointLines;
public:
	IKExampleTestBase();

protected:
	virtual void onCreate() override;
	virtual void onUpdate(float dt) override;
	virtual void onRender(float aspect) override;

protected:
	void _createIKChains();
	void _createLoopingClip();

	SPtr<DebugDrawPL>			_ikChainsVisual;
	Vector<UPtr<DebugDraw>, 3>	_targetVisual;

	Track_SampleRequest			_sr;

	Transform					_target;
	TransformTrack				_targetPath;

	UPtr<IKSolver>				_solver;
};

}