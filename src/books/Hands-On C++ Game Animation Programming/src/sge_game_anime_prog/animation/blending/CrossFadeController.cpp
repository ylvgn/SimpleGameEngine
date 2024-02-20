#include "CrossFadeController.h"
#include "Blending.h"

namespace sge {

CrossFadeController* CrossFadeController::_instance = nullptr;

CrossFadeController::CrossFadeController()
	: _curClip(nullptr)
	, _curPlaybackTime(0)
	, _curSkeleton(nullptr)
	, _isSetSkeleton(false)
{
	SGE_ASSERT(_instance == nullptr);
	_instance = this;
}

CrossFadeController::~CrossFadeController() {
	SGE_ASSERT(_instance == this);
	_instance = nullptr;
}

void CrossFadeController::setSkeleton(Skeleton* skeleton) {
	if (_curSkeleton == skeleton) return;
	_curSkeleton = skeleton;

	if (skeleton) {
		_curPose = skeleton->restPose();
	}
	_isSetSkeleton = skeleton != nullptr;
}

void CrossFadeController::play(const Clip* target) {
	if (!_isSetSkeleton) return;
	SGE_ASSERT(_curSkeleton != nullptr);

	_curClip = target;
	_curPose = _curSkeleton->restPose();
	_curPlaybackTime = 0.f;
	_fadingTargets.clear();
}

void CrossFadeController::fadeTo(const Clip* target, float fadeDuration) {
	// A fade target is only valid if it is not the first or last item in the fade list.

	if (_curClip == nullptr) {
		play(target); // assign to _curClip
		return;
	}

	if (_fadingTargets.empty()) {
		if (_curClip == target) { // blending to same _curClip
			return;
		}
	} else {
		const auto* clip = _fadingTargets.back()->clip;
		if (target == clip) { // last clip is same _curClip
			return;
		}
	}

	SGE_ASSERT(_curSkeleton != nullptr);
	_fadingTargets.emplace_back(new MyTarget(target, _curSkeleton->restPose(), fadeDuration));
}

void CrossFadeController::update(float dt) {
	if (_curClip == nullptr || !_isSetSkeleton) {
		return;
	}

	SGE_ASSERT(_curSkeleton != nullptr);

	int i = 0;
	for (const auto& target : _fadingTargets) {
		float fadeDuration    = target->fadeDuration;
		float elapsedFadeTime = target->elapsedFadeTime;

		if (elapsedFadeTime >= fadeDuration) {
			_curClip		 = target->clip;
			_curPlaybackTime = target->playbackTime;
			_curPose		 = target->pose;
			_fadingTargets.erase(_fadingTargets.begin() + i);
			break;
		}
		++i;
	}

	_curPlaybackTime = _curClip->sample(_curPose, _curPlaybackTime + dt);
	for (auto& target : _fadingTargets) {
		target->clip->sample(target->pose, target->playbackTime + dt);
		target->elapsedFadeTime += dt;
		float t = target->elapsedFadeTime / target->fadeDuration;
		t = Math::clamp01(t);
		Blending::blend(_curPose, _curPose, target->pose, t, -1);
	}
}

}