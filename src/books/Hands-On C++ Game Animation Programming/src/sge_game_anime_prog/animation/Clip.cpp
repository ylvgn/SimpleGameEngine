#include "Clip.h"

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
float ClipT<TRACK>::sample(Pose& out, float time) const {
// The sample function takes a Pose reference and a time and returns a float value that is also a time.
	if (getDuration() == 0.f) {
		return 0;
	}

	// make sure the clip is valid and that the time is in the range of the clip.
	time = _adjustTimeToFitRange(time);

	SampleRequest sr;
	sr.isLoop = _isLoop;
	
	for (auto& track : _tracks) {
		Transform local = out.getLocalTransform(track->jointId);

		// If a component of a transform isn't animated,
		// the reference components are used to provide default values.

		sr.time = time;
		Transform animated = track->sample(local, sr);

		out.setLocalTransform(track->jointId, animated);
	}

	return time;
}

template<typename TRACK>
TRACK* ClipT<TRACK>::findTrackByJointId(u32 jointId) {
	for (auto& tracks : _tracks) {
		if (tracks->jointId == jointId) {
			return tracks.get();
		}
	}
	return nullptr;
}

template<typename TRACK>
void ClipT<TRACK>::recalculateDuration() {
// a public helper function to figure out the start and end times of the animation clip.
// This function is intended to be called by the code that loads the animation clip from a file format.
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

// returns a transform track for the specified track(joint).
// If no track exists for the specified track(joint), one is created and returned.
template<typename TRACK>
TRACK& ClipT<TRACK>::getOrCreateTrackByJointId(u32 jointId) {
	// This function is mainly used by whatever code loads the animation clip from a file.
	TRACK* p = findTrackByJointId(jointId);
	if (p) return *p;

	UPtr<TRACK> newTransformTrack = s_createTrack(jointId);
	_tracks.reserve(_tracks.size() + 1);
	_tracks.emplace_back(std::move(newTransformTrack));
	auto& res = _tracks.back();
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

	for (int i = 0; i < src.getTrackCount(); i++) {
		auto* p = src.getTrack_noCheck(i);
		UPtr<FastTransformTrack> newTransformTrack = TransformTrackUtil::optimizeTransformTrack(*p);
		res.appendTrack(std::move(newTransformTrack));
	}

	res.setName(src.name());
	res.setIsLoop(src.isLoop());
	res.recalculateDuration();
	return res;
}

}

