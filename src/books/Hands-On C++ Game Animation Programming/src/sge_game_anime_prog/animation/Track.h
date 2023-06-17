#pragma once

#include "Interpolation.h"
#include "Frame.h"
#include <sge_game_anime_prog/math/vec3.h>
#include <sge_game_anime_prog/math/quat.h>

namespace sge {

/*
	An animation track is a collection of keyframes.
	Interpolating a track returns the data type of the track;
	the result is the value along whatever curve the track defines at a specific point in time.
	A track must have at least two frames to interpolate between.
*/

struct Track_SampleRequest {
	float time = 0.f;
	bool isLoop = false;
};

template<typename T, size_t N>
class Track {
	using SampleRequest = Track_SampleRequest;
public:

	static constexpr int kInvalidFrameIndex = -1;

	static T s_hermite(float t, const T& p1, const T& s1, const T& p2, const T& s2);

	Track() = default;

	inline size_t size() const		{ return _frames.size(); }
	void clear()					{ _frames.clear(); }
	void resize(size_t frameCount)	{ _frames.resize(frameCount); }

	inline float getStartTime()		const { SGE_ASSERT(isValid()); return _frames[0].time; };
	inline float getEndTime()		const { SGE_ASSERT(isValid()); return _frames[_frames.size() - 1].time; };
	inline float getDuration()		const { return getEndTime() - getStartTime(); }

	inline float saveGetStartTime() const { return isValid() ? _frames[0].time : 0; }
	inline float saveGetEndTime()   const { return isValid() ? _frames[_frames.size() - 1].time : 0; }
	inline float saveGetDuration()  const { return isValid() ? saveGetStartTime() - saveGetEndTime() : 0; }

	// For a track to be valid, it needs to have two or more frames
	inline bool isValid()  const { return _frames.size() > 1; }
	inline bool isValid2() const { return isValid() && getDuration() > 0; }

	T sample(const SampleRequest& sr) const;

	      Frame<N>& operator[] (int i)       { return _frames[i]; }
	const Frame<N>& operator[] (int i) const { return _frames[i]; }

	// keeps the animation sampling time in the valid range between startTime~endTime
	float adjustTimeToFitTrack(const SampleRequest& sr) const;

	int getFrameIndex(const SampleRequest& sr) const;

	inline Interpolation type()	const { return _type; }
	inline void setInterpolation(Interpolation type) { _type = type; }

	static Track<T, N> s_createTrack(Interpolation type, int numFrames, ...) {
		
		using VA_ARG_Type = Frame<N>;

		Track<T, N> result;
		result.setInterpolation(type);
		result.resize(numFrames);

		va_list args;
		va_start(args, numFrames);
		for (int i = 0; i < numFrames; ++i) {
			result[i] = va_arg(args, VA_ARG_Type);
		}
		va_end(args);

		return result;
	}

private:

	static T s_toValue(const float* v);

	T _sampleConstant(const SampleRequest& sr) const;
	T _sampleLinear  (const SampleRequest& sr) const;
	T _sampleCubic   (const SampleRequest& sr) const;

	Interpolation _type = Interpolation::Linear;
	Vector< Frame<N>, 220 > _frames;
};

using ScalarTrack		= Track<float, 1>;
using VectorTrack		= Track<vec3,  3>;
using QuaternionTrack	= Track<quat,  4>;

}