#pragma once

#include "ExampleTestBase.h"

#include <sge_game_anime_prog/math/Transform.h>
#include <sge_game_anime_prog/animation/Track.h>
#include <sge_game_anime_prog/animation/TransformTrack.h>

namespace sge {

class DebugDraw;

template<class IKSolver>
class IKExampleTestBase : public ExampleTestBase {
	using Base = ExampleTestBase;
public:
	IKExampleTestBase();

	constexpr static Color4f kWhite  =  {1,1,1,1};
	constexpr static Color4f kRed	 =	{1,0,0,1};
	constexpr static Color4f kGreen  =	{0,1,0,1};
	constexpr static Color4f kBlue	 =	{0,0,1,1};
	constexpr static Color4f kYellow =	{1,1,0,1};
	constexpr static Color4f kPurple =	{1,0,1,1};

protected:
	virtual void onCreate() override;
	virtual void onUpdate(float dt) override;
	virtual void onRender(float aspect) override;
	
protected:
	void _createIKChains();
	void _createLoopingClip();

	UPtr<DebugDraw>				_debugPoints;
	UPtr<DebugDraw>				_debugLines;
	Vector<UPtr<DebugDraw>, 3>	_targetVisual;

	Track_SampleRequest			_sr;

	Transform					_target;
	TransformTrack				_targetPath;

	UPtr<IKSolver>				_solver;
};

}