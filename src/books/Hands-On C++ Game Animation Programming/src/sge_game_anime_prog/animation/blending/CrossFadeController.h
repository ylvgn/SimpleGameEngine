#pragma once

#include <sge_core/pointer/SPtr.h>

#include "../Clip.h"
#include "../Pose.h"
#include "../Skeleton.h"

namespace sge {

/*
	Another common case for blending animations -- Crossfading animations

	A crossfade is a fast blend from one animation to another.
	The goal of the crossfade is to hide the transition between two animations.
*/

class CrossFadeController : public NonCopyable {
private:

	struct MyTarget : public NonCopyable {
		MyTarget() = default;
		MyTarget(const Clip* clip_, const Pose& pose_, float fadeDuration_)
			: clip(clip_)
			, pose(pose_)
			, playbackTime(clip->getStartTime())
			, fadeDuration(fadeDuration_)
			, elapsedFadeTime(0.f) {}

		// used to per frame sample
		const Clip* clip = nullptr;
		Pose		pose;
		float		playbackTime = 0.f;

		// used to control how much the animation should be faded in.
		float		fadeDuration = 0.f;
		float		elapsedFadeTime = 0.f;
	};

public:

	static CrossFadeController* instance() { return _instance; }

	CrossFadeController();
	~CrossFadeController();

	void setSkeleton(Skeleton* skeleton);

	void play(const Clip* target);
	void fadeTo(const Clip* target, float fadeDuration);
	void update(float dt);

	const Pose& curPose() const { return _curPose; }

private:

	static CrossFadeController* _instance;

	const Clip*					_curClip = nullptr;
	float						_curPlaybackTime;
	Pose						_curPose;

	SPtr<Skeleton>				_curSkeleton;
	bool						_isSetSkeleton;

	// The entire '_fadingTargets' list is evaluated with every frame.
	// Each animation is evaluated and blended into the currently playing animation.
	Vector< UPtr<MyTarget> >	_fadingTargets;
};

}