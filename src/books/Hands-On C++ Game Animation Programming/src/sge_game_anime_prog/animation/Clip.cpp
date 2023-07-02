#include "Clip.h"
#include "TransformTrack.h"

namespace sge {

template<typename TRACK>
float TClip<TRACK>::_adjustTimeToFitRange(float time) const {
	// same logic as Track.h 's _adjustTimeToFitTrack function

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
void TClip<TRACK>::recalculateDuration() {
	_startTime = 0.f;
	_endTime   = 0.f;

	if (_tracks.size() == 0) return;

	_startTime = _tracks[0].getStartTime();
	_endTime   = _tracks[0].getEndTime();

	for (int i = 1; i < _tracks.size(); ++i) {
		_startTime	= Math::min(_startTime, _tracks[i].getStartTime());
		_endTime	= Math::max(_endTime, _tracks[i].getEndTime());
	}
/*
	The start time of a clip might not be 0;
	it's possible to have a clip that starts at an arbitrary point in time
*/
}

template<typename TRACK>
float TClip<TRACK>::sample(Pose& out, float time) const {
	if (getDuration() == 0.f) {
		return 0;
	}

	// make sure the clip is valid and that the time is in the range of the clip.
	time = _adjustTimeToFitRange(time);

	Track_SampleRequest sr;
	sr.isLoop = _isLoop;

	for (int i = 0; i < _tracks.size(); ++i) {
		u32 joint = _tracks[i].id();
		Transform local = out.getLocalTransform(joint);

		// If a component of a transform isn't animated,
		// the reference components are used to provide default values.

		sr.time = time;
		Transform animated = _tracks[i].sample(local, sr);

		out.setLocalTransform(joint, animated);
	}

	return time;
}

// returns a transform track for the specified track(joint).
// If no track exists for the specified track(joint), one is created and returned.
template<typename TRACK>
TRACK& TClip<TRACK>::operator[] (u32 jointId) {
	// This function is mainly used by whatever code loads the animation clip from a file.

	for (int i = 0; i < _tracks.size(); ++i) {
		if (_tracks[i].id() == jointId) {
			return _tracks[i];
		}
	}

	_tracks.push_back(TRACK());
	auto& res = _tracks.back();
	res.setId(jointId);
	return res;
}

template TClip<TransformTrack>;
template TClip<FastTransformTrack>;

FastClip ClipUtil::optimizeClip(const Clip& src) {
	FastClip res;
	res.setName(src.name());
	res.setIsLoop(src.isLoop());

	for (const auto& t : src.tracks()) {
		res.appendTrack(TransformTrackUtil::optimizeTransformTrack(t));
	}
	res.recalculateDuration();
	return res;
}

}

