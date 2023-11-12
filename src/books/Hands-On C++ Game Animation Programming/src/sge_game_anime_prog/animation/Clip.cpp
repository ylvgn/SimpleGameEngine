#include "Clip.h"
#include "TransformTrack.h"

namespace sge {

template<typename TRACK>
float ClipT<TRACK>::_adjustTimeToFitRange(float time) const {
	// same logic as Track.h '_adjustTimeToFitTrack' function

	float duration = getDuration();
	if (duration <= 0) {
		return 0;
	}

	if (_isLoop) {
		time = Math::fmod(time - _startTime, duration);
		if (time < 0.f) {
			time += duration;
		}
		time += _startTime;
	} else {
		time = Math::clamp(time, _startTime, _endTime);
	}

	return time;
}

template<typename TRACK>
void ClipT<TRACK>::recalculateDuration() {
	_startTime = 0.f;
	_endTime   = 0.f;

	size_t trackCount = getTrackCount();
	if (trackCount == 0) return;

	_startTime = _tracks[0]->getStartTime();
	_endTime   = _tracks[0]->getEndTime();

	for (int i = 1; i < _tracks.size(); ++i) {
		_startTime	= Math::min(_startTime, _tracks[i]->getStartTime());
		_endTime	= Math::max(_endTime, _tracks[i]->getEndTime());
	}
/*
	The start time of a clip might not be 0;
	it's possible to have a clip that starts at an arbitrary point in time
*/
}

template<typename TRACK>
float ClipT<TRACK>::sample(Pose& out, float time) const {
	if (getDuration() == 0.f) {
		return 0;
	}

	// make sure the clip is valid and that the time is in the range of the clip.
	time = _adjustTimeToFitRange(time);

	SampleRequest sr;
	sr.isLoop = _isLoop;
	
	for (auto& track : _tracks) {
		u32 jointId		= track->id();
		Transform local = out.getLocalTransform(jointId);

		// If a component of a transform isn't animated,
		// the reference components are used to provide default values.

		sr.time = time;
		Transform animated = track->sample(local, sr);

		out.setLocalTransform(jointId, animated);
	}

	return time;
}

// returns a transform track for the specified track(joint).
// If no track exists for the specified track(joint), one is created and returned.
template<typename TRACK>
TRACK& ClipT<TRACK>::operator[] (u32 jointId) {
	// This function is mainly used by whatever code loads the animation clip from a file.

	for (int i = 0; i < _tracks.size(); ++i) {
		if (_tracks[i]->id() == jointId) {
			return *_tracks[i].get();
		}
	}
	_tracks.reserve(_tracks.size() + 1);
	UPtr<TRACK> newTransformTrack = eastl::make_unique<TRACK>(); // do not use auto type, use explicit UPtr<T>
	_tracks.push_back(std::move(newTransformTrack));
	auto& res = _tracks.back();
	res->setId(jointId);
	return *res.get();
}

template ClipT<TransformTrack>;
template ClipT<FastTransformTrack>;

#if 0
#pragma mark ================= ClipUtil ====================
#endif
FastClip ClipUtil::optimizeClip(const Clip& src) {
	FastClip res;
	res.reserve(src.getTrackCount());
	for (auto& t : src.tracks()) {
		UPtr<FastTransformTrack> fast = TransformTrackUtil::optimizeTransformTrack(*t.get());
		res.appendTrack(std::move(fast));
	}
	res.setName(src.name());
	res.setIsLoop(src.isLoop());
	res.recalculateDuration();
	return res;
}

}

