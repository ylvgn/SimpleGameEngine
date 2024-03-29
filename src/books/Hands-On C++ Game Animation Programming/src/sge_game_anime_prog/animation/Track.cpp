#include "Track.h"

#include <sge_game_anime_prog/math/quat.h>

namespace sge {

#if 0
#pragma mark ================= TrackHelpers ====================
#endif
namespace TrackHelpers {
/*
	These helper functions are not a part of the Track class;
	they rely on function overloading to make sure the right version of the helper function is called.
	One of the key responsibilities of these helper classes is to 
		- make sure quaternions are normalized
		- and are in the right neighborhood.
*/

inline float	interpolate(float a, float b, double t)				   { return static_cast<float>(a + (b-a)*t); }
inline float	interpolate(float a, float b, float t)				   { return Math::lerp(a,b,t); }
inline vec3f	interpolate(const vec3f& a, const vec3f& b, float t)   { return a.lerp(b, t); }
inline quat4f	interpolate(const quat4f& a, const quat4f& b, float t) { // nlerp
	quat4f q = a.mix(b, t);
	if (a.dot(b) < 0) {
		// neighborhood
		q = a.mix(-b, t);
	}
	return q.normalize();
}

inline float  adjustHermiteResult(float f)			{ return f; }
inline vec3f  adjustHermiteResult(const vec3f& v)	{ return v; }
inline quat4f adjustHermiteResult(const quat4f& q)	{ return q.normalize(); }

inline void neighborhood(const float& a, float& b)	{}
inline void neighborhood(const vec3f& a, vec3f& b)	{}
inline void neighborhood(const quat4f& a, quat4f& b){ if (a.dot(b) < 0) b = -b; }

} // TrackHelpers namespace

#if 0
#pragma mark ================= Track ====================
#endif
template<> float  Track<float, 1>::s_toValue(const float* v) { return *v; }
template<> vec3f  Track<vec3f, 3>::s_toValue(const float* v) { return vec3f(v[0], v[1], v[2]); }
template<> quat4f Track<quat4f,4>::s_toValue(const float* v) { return quat4f(v[0], v[1], v[2], v[3]).normalize(); }

template<typename T, size_t N>
T Track<T, N>::sample(const SampleRequest& sr) const {
	switch (_type)
	{
		case Interpolation::Constant:	return _sampleConstant(sr);
		case Interpolation::Linear:		return _sampleLinear(sr);
		case Interpolation::Cubic:		return _sampleCubic(sr);
		default:						throw SGE_ERROR("unsupported Interpolation type: {}", _type);
	}
}

template<typename T, size_t N>
T Track<T, N>::_sampleConstant(const SampleRequest& sr) const {
	int i = getFrameIndex(sr);

	// Make sure the frame is valid
	if (i == kInvalidFrameIndex || i >= _frames.size() - 1) {
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

	float curTrackTime = _adjustTimeToFitTrack(sr);
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
	const auto& nextFrame = _frames[i+1];

	float trackStartTime  = thisFrame.time;
	float trackEndTime    = nextFrame.time;
	float trackDeltaTime  = trackEndTime - trackStartTime;

	if (trackDeltaTime <= 0.0f) {
		return T();
	}
	float curTrackTime = _adjustTimeToFitTrack(sr);
	float t = (curTrackTime - trackStartTime) / trackDeltaTime;

	T p1 = Track<T, N>::s_toValue(thisFrame.value);
	T p2 = Track<T, N>::s_toValue(nextFrame.value);

	// slopes are not meant to be quaternions, use memcpy directly instead of s_toValue avoiding normalization.
	// Both slopes need to be scaled by the frame delta
	// slope1 = p1.out * frameDelta
	T slope1;
	memcpy(&slope1, thisFrame.out, N * sizeof(float));
	slope1 *= trackDeltaTime;

	// slope2 = p2.in * frameDelta 
	T slope2;
	memcpy(&slope2, nextFrame.in, N * sizeof(float));
	slope2 *= trackDeltaTime;

	T result = TrackUtil::hermite(t, p1, slope1, p2, slope2);
	return TrackHelpers::adjustHermiteResult(result);
}

template<typename T, size_t N>
int Track<T, N>::getFrameIndex(const SampleRequest& sr) const {
	size_t frameCount = _frames.size();

	if (frameCount <= 1) {
		return kInvalidFrameIndex;
	}

#if 1
	float time      = sr.time;
	float startTime = getStartTime();
	float endTime   = getEndTime();

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
			return static_cast<int>(frameCount) - 2;
		}
	}
#else
	float time = _adjustTimeToFitTrack(sr);
	if (!sr.isLoop) {
		float secondToLastTime = _frames[_frames.size() - 2].time;
		if (time >= secondToLastTime) {
			return static_cast<int>(frameCount) - 2;
		}
	}
#endif

	// This frame can be found by looping through the frames of the track backwardand returning the first index
	// whose time is less than the time that is looked up
	for (size_t i = frameCount - 1; i >= 0; --i) {
		if (time >= _frames[i].time)
			return static_cast<int>(i);
	}

	throw SGE_ERROR("Invalid code, we should not reach here!");
}

template<typename T, size_t N>
float Track<T, N>::_adjustTimeToFitTrack(const SampleRequest& sr) const {
	size_t frameCount = _frames.size();
	if (frameCount <= 1) return 0;

	float time		= sr.time;
	float startTime = getStartTime();
	float endTime	= getEndTime();
	float duration	= endTime - startTime;

	if (duration <= 0) {
		return 0;
	}

	if (sr.isLoop) {
		time = Math::fmod(time - startTime, duration);
		if (time < 0) {
			time += duration;
		}
		time += startTime;
	} else {
		time = Math::clamp(time, startTime, endTime);
	}
	
	return time;
}

template Track<float,  1>;
template Track<vec3f,  3>;
template Track<quat4f, 4>;

#if 0
#pragma mark ================= FastTrack ====================
#endif
template<typename T, size_t N>
void FastTrack<T, N>::updateIndexLookupTable() {
/*
	The updateIndexLookupTable function is intended to be called at load time.

	How many samples should the FastTrack class contain?
	This question is very context-dependent as different games have different requirements.
	For the context of this book, 60 samples per second should be enough
*/
	size_t frameCount = _frames.size();
	if (frameCount <= 1) return;

	float startTime = getStartTime();
	float endTime   = getEndTime();
	float duration  = endTime - startTime;

	// Since the class has 60 samples for every second of animation, multiply the duration by 60
	int sampleCount = _getSampleCount();
	_sampled2FrameIndex.resize(sampleCount);

	for (int i = 0; i < sampleCount; ++i) {
		float t = static_cast<float>(i) / static_cast<float>(sampleCount - 1); // normalized sample time
		float time = startTime + (t*duration);

		size_t frameIndex = 0;
		for (size_t j = frameCount - 1; j >= 0; --j) {
			const Frame<N>& frame = _frames[j];
			if (time >= frame.time) {
				frameIndex = j;
				break;
			}
		}
		_sampled2FrameIndex[i] = static_cast<int>(Math::min(frameIndex, frameCount - 2));
	}
}

template<typename T, size_t N>
int FastTrack<T, N>::getFrameIndex(const SampleRequest& sr) const {
	// The optimized FastTrack class uses a lookup array instead of looping through every frame of the track.
	size_t frameCount = _frames.size();
	if (frameCount <= 1) return kInvalidFrameIndex;

	float time      = sr.time;
	float startTime = getStartTime();
	float endTime   = getEndTime();
	float duration  = endTime - startTime;

	if (sr.isLoop) {
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
			return static_cast<int>(frameCount) - 2;
		}
	}

	float t = time / duration; // normalized sample time
	int index = static_cast<int>(t * _getSampleCount());
	if (index >= _sampled2FrameIndex.size()) {
		return kInvalidFrameIndex;
	}
	return _sampled2FrameIndex[index];
}

template FastTrack<float,  1>;
template FastTrack<vec3f,  3>;
template FastTrack<quat4f, 4>;

#if 0
#pragma mark ================= TrackUtil ====================
#endif
template<typename T>
T TrackUtil::hermite(float t, const T& p1, const T& s1, const T& p2_, const T& s2) {
	float tt	= t*t;
	float tt2	= tt*2;
	float tt3	= tt*3;
	float ttt	= tt*t;
	float ttt2	= ttt*2;

	T p2 = p2_;
	TrackHelpers::neighborhood(p1, p2);

	return p1*(ttt2-tt3+1) + s1*(ttt-tt2+t) + p2*(-ttt2+tt3) + s2*(ttt-tt);
}

// explicit instantiation
// Declare the template specializations of the optimizeTrack function for all three types
// This means declaring specializations that work with the scalar, vector 3, and quaternion tracks
template FastTrack<float, 1> TrackUtil::optimizeTrack(const Track<float, 1>& src);
template FastTrack<vec3f, 3> TrackUtil::optimizeTrack(const Track<vec3f, 3>& src);
template FastTrack<quat4f,4> TrackUtil::optimizeTrack(const Track<quat4f,4>& src);

template<typename T, size_t N>
FastTrack<T, N> TrackUtil::optimizeTrack(const Track<T, N>& src) {
	FastTrack<T, N> res;

	res.setType(src.type());
	size_t frameCount = src.getFrameCount();
	res.resize(frameCount);
	for (int i = 0; i < frameCount; ++i) {
		res[i] = src[i];
	}

	res.updateIndexLookupTable();
	return res;
}

} // namespace