#include "Track.h"

namespace sge {

template Track <float, 1>;
template Track <vec3,  3>;
template Track <quat,  4>;

namespace TrackHelpers {
/*
	These helper functions are not a part of the Track class;
	they rely on function overloading to make sure the right version of the helper function is called.
	One of the key responsibilities of these helper classes is to 
		- make sure quaternions are normalized
		- and are in the right neighborhood.
*/

inline float interpolate(float a, float b, float t) { return a + (b-a)*t; }
inline vec3  interpolate(const vec3& a, const vec3& b, float t) { return a.lerp(b, t); }
inline quat  interpolate(const quat& a, const quat& b, float t) { // nlerp
	quat q = a.mix(b, t);
	if (a.dot(b) < 0) { // neighborhood
		q = a.mix(-b, t);
	}
	return q.normalize();
}

inline float adjustHermiteResult(float f) { return f; }
inline vec3  adjustHermiteResult(const vec3& v) { return v; }
inline quat  adjustHermiteResult(const quat& q) { return q.normalize(); }

inline void neighborhood(const float& a, float& b) { }
inline void neighborhood(const vec3& a, vec3& b) { }
inline void neighborhood(const quat& a, quat& b) { if (a.dot(b) < 0) b = -b; }

} // TrackHelpers namespace

template<>
float Track<float, 1>::s_toValue(const float* v) { return *v; }

template<>
vec3 Track<vec3, 3>::s_toValue(const float* v) { return vec3(v[0], v[1], v[2]); }

template<>
quat Track<quat, 4>::s_toValue(const float* v) { return quat(v[0], v[1], v[2], v[3]).normalize(); }

template<typename T, size_t N>
T Track<T, N>::sample(const SampleRequest& sr) const {
	switch (_type)
	{
		case Interpolation::Constant:	return _sampleConstant(sr);
		case Interpolation::Linear:		return _sampleLinear(sr);
		case Interpolation::Cubic:		return _sampleCubic(sr);
		default:						throw SGE_ERROR("not supported");
	}
}

template<typename T, size_t N>
T Track<T, N>::_sampleConstant(const SampleRequest& sr) const {
	int i = getFrameIndex(sr);

	// Make sure the frame is valid
	if (i == kInvalidFrameIndex || i >= _frames.size()) {
		return T();
	}
	SGE_ASSERT(i >= 0);

	return Track<T, N>::s_toValue(_frames[i].value);
}

template<typename T, size_t N>
T Track<T, N>::_sampleLinear(const SampleRequest& sr) const {
	int i = getFrameIndex(sr);

	// Make sure the frame is valid
	if (i == kInvalidFrameIndex || i >= _frames.size() - 1) {
		return T();
	}
	SGE_ASSERT(i >= 0);

	const auto& thisFrame = _frames[i];
	const auto& nextFrame = _frames[i + 1];

	float trackStartTime = thisFrame.time;
	float trackEndTime   = nextFrame.time;
	float trackDeltaTime = trackEndTime - trackStartTime;

	if (trackDeltaTime <= 0.0f) {
		return T();
	}

	float curTrackTime = adjustTimeToFitTrack(sr);
	float t = (curTrackTime - trackStartTime) / trackDeltaTime;

	T a = Track<T, N>::s_toValue(thisFrame.value);
	T b = Track<T, N>::s_toValue(nextFrame.value);

	return TrackHelpers::interpolate(a, b, t);
}

template<typename T, size_t N>
T Track<T, N>::_sampleCubic(const SampleRequest& sr) const {
	int i = getFrameIndex(sr);

	// Make sure the frame is valid
	if (i == kInvalidFrameIndex || i >= _frames.size() - 1) {
		return T();
	}
	SGE_ASSERT(i >= 0);

	const auto& thisFrame = _frames[i];
	const auto& nextFrame = _frames[i + 1];

	float trackStartTime = thisFrame.time;
	float trackEndTime   = nextFrame.time;
	float trackDeltaTime = trackEndTime - trackStartTime;

	if (trackDeltaTime <= 0.0f) {
		return T();
	}
	float curTrackTime = adjustTimeToFitTrack(sr);
	float t = (curTrackTime - trackStartTime) / trackDeltaTime;

	T p1 = Track<T, N>::s_toValue(thisFrame.value);
	T p2 = Track<T, N>::s_toValue(nextFrame.value);

	// slopes are not meant to be quaternions, use memcpy directly instead of s_toValue avoiding normalization.
	// Both slopes need to be scaled by the frame delta ???
	// slope1 = p1.out * frameDelta
	T slope1;
	std::memcpy(&slope1, thisFrame.out, N * sizeof(float));
	slope1 = slope1 * trackDeltaTime;

	// s2 = p2.in * frameDelta 
	T slope2;
	std::memcpy(&slope2, nextFrame.in, N * sizeof(float));
	slope2 = slope2 * trackDeltaTime;

	T result = Track<T, N>::s_hermite(t, p1, slope1, p2, slope2);
	return TrackHelpers::adjustHermiteResult(result);
}

template<typename T, size_t N>
int Track<T, N>::getFrameIndex(const SampleRequest& sr) const {
	int frameCount = static_cast<int>(_frames.size());

	if (frameCount <= 1) {
		return kInvalidFrameIndex;
	}

#if 0
	float time = sr.time;
	float startTime = getStartTime();
	float endTime = getEndTime();

	if (sr.isLoop) {
		float duration = endTime - startTime;
		SGE_ASSERT(duration > 0);
		time = Math::fmod(time - startTime, duration);
		if (time < 0) {
			time += duration;
		}
		time += startTime;
	}
	else {
		if (time <= startTime) {
			return 0;
		}

		float secondToLastTime = _frames[_frames.size() - 2].time;
		if (time >= secondToLastTime) {
			// The sample function always needs a currentand next frame, and the next frame is found by adding 1 to the result of the FrameIndex function.
			return frameCount - 2;
		}
	}
#else
	float time = adjustTimeToFitTrack(sr);
	if (!sr.isLoop) {
		float secondToLastTime = _frames[_frames.size() - 2].time;
		if (time >= secondToLastTime) {
			return frameCount - 2;
		}
	}
#endif
	// This frame can be found by looping through the frames of the track backwardand returning the first index
	// whose time is less than the time that is looked up
	for (int i = frameCount - 1; i >= 0; --i) {
		if (time >= _frames[i].time)
			return i;
	}

	throw SGE_ERROR("Invalid code, we should not reach here!");
}

template<typename T, size_t N>
float Track<T, N>::adjustTimeToFitTrack(const SampleRequest& sr) const {
	int frameCount = static_cast<int>(_frames.size());

	if (frameCount <= 1) {
		return 0;
	}

	float time = sr.time;
	float startTime = getStartTime();
	float endTime = getEndTime();
	float duration = endTime - startTime;

	if (duration <= 0) {
		return 0;
	}

	if (sr.isLoop) {
		time = Math::fmod(time - startTime, duration);
		if (time < 0) {
			time += duration;
		}
		time += startTime;
	}
	else {
		time = Math::clamp(time, startTime, endTime);
	}
	
	return time;
}

template<typename T, size_t N>
T Track<T, N>::s_hermite(float t, const T& p1, const T& s1, const T& p2_, const T& s2) {
	float tt   = t * t;
	float tt2  = tt * 2;
	float tt3  = tt * 3;
	float ttt  = tt * t;
	float ttt2 = ttt * 2;

	T p2 = constCast(p2_);
	TrackHelpers::neighborhood(p1, p2);

	return p1*(ttt2-tt3+1) + s1*(ttt-tt2+t) + p2*(-ttt2+tt3) + s2*(ttt-tt);
}

}