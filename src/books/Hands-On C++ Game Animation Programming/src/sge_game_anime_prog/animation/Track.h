#pragma once

#include "Interpolation.h"
#include "Frame.h"

namespace sge {
/*
	An animation track is a collection of keyframes.
	Interpolating a track returns the data type of the track;
	the result is the value along whatever curve the track defines at a specific point in time.
	A track must have at least two frames to interpolate between.
*/

struct Track_SampleRequest {
	Track_SampleRequest()
		: time(0)
		, isLoop(false)
	{}

	float time;
	bool  isLoop : 1;
};

template<typename T, size_t N>
struct Track {
public:
	using SampleRequest = Track_SampleRequest;
	using Type			= Interpolation;

	constexpr static int kInvalidFrameIndex = -1;

	void clear()					{ _frames.clear(); }
	void resize(size_t frameCount)	{ _frames.resize(frameCount); }

	size_t getFrameCount()	 const { return _frames.size(); }

	float getStartTime()	 const { SGE_ASSERT(isValid()); return _frames[0].time; }
	float getEndTime()		 const { SGE_ASSERT(isValid()); return _frames[_frames.size() - 1].time; };
	float getDuration()		 const { return getEndTime() - getStartTime(); }

	float saveGetStartTime() const { return isValid() ? _frames[0].time : 0; }
	float saveGetEndTime()   const { return isValid() ? _frames[_frames.size() - 1].time : 0; }
	float saveGetDuration()  const { return isValid() ? saveGetStartTime() - saveGetEndTime() : 0; }

	// For a track to be valid, it needs to have two or more frames
	bool isValid()  const { return _frames.size() > 1; }
	bool isValid2() const { return isValid() && getDuration() > 0; }

	T sample(const SampleRequest& sr) const;

	      Frame<N>& operator[] (int i)       { return _frames[i]; }
	const Frame<N>& operator[] (int i) const { return _frames[i]; }

	Type type() const		 { return _type; }
	void setType(Type type)  { _type = type; }

protected:
	virtual int getFrameIndex(const SampleRequest& sr) const;

	static T s_toValue(const float* v);

	// keeps the animation sampling time in the valid range between startTime~endTime
	float _adjustTimeToFitTrack(const SampleRequest& sr) const;

	T _sampleConstant(const SampleRequest& sr) const;
	T _sampleLinear  (const SampleRequest& sr) const;
	T _sampleCubic   (const SampleRequest& sr) const;

	Interpolation			_type = Interpolation::Linear;
	Vector< Frame<N>, 220 > _frames;
};

using ScalarTrack		= Track<float, 1>;
using VectorTrack		= Track<vec3f, 3>;
using QuaternionTrack	= Track<quat4f,4>;

#if 0
#pragma mark ------------------- FastTrack -------------------
#endif
template<typename T, size_t N>
struct FastTrack : public Track<T, N> {
public:
	using Base = Track<T, N>;
	constexpr static int kFrameCountPerSecond = 60;

	void updateIndexLookupTable();

protected:
	// track sampling this way is greater when the animation in question has more frames.
	virtual int getFrameIndex(const SampleRequest& sr) const override;

private:
	constexpr int _getSampleCount() const {
		float duration = getDuration();
		SGE_ASSERT(duration >= 0);
		return kFrameCountPerSecond + (static_cast<int>(duration * kFrameCountPerSecond));
	}

	Vector<int> _sampled2FrameIndex;
};

using FastScalarTrack		= FastTrack<float, 1>;
using FastVectorTrack		= FastTrack<vec3f, 3>;
using FastQuaternionTrack	= FastTrack<quat4f,4>;

#if 0
#pragma mark ------------------- TrackUtil -------------------
#endif
struct TrackUtil {
	TrackUtil() = delete;

	using Type = Interpolation;

	template<typename T, size_t N>
	static Track<T, N> createTrack(Type type, size_t frameCount, ...);

	template<class... Args>
	static ScalarTrack createScalarTrack(Type type, size_t frameCount, Args&&... args);

	template<class... Args>
	static VectorTrack createVectorTrack(Type type, size_t frameCount, Args&&... args);

	template<class... Args>
	static QuaternionTrack createQuaternionTrack(Type type, size_t frameCount, Args&&... args);

	template<typename T>
	static T hermite(float t, const T& p1, const T& s1, const T& p2, const T& s2);

	template<typename T, size_t N>
	static FastTrack<T, N> optimizeTrack(const Track<T, N>& src);
};

template<typename T, size_t N> inline
static Track<T, N> TrackUtil::createTrack(Type type, size_t frameCount, ...) {
	using VA_ARG_Type = Frame<N>;

	Track<T, N> result;
	result.setType(type);
	result.resize(frameCount);

	va_list args;
	va_start(args, frameCount);
	for (int i = 0; i < frameCount; ++i) {
		result[i] = va_arg(args, VA_ARG_Type);
	}
	va_end(args);

	return result;
}

template<class... Args> inline
static ScalarTrack TrackUtil::createScalarTrack(Type type, size_t frameCount, Args&&... args) {
	return TrackUtil::createTrack<float, 1>(type, frameCount, SGE_FORWARD(args)...);
}

template<class... Args> inline
static VectorTrack TrackUtil::createVectorTrack(Type type, size_t frameCount, Args&&... args) {
	return TrackUtil::createTrack<vec3f, 3>(type, frameCount, SGE_FORWARD(args)...);
}

template<class... Args> inline
static QuaternionTrack TrackUtil::createQuaternionTrack(Type type, size_t frameCount, Args&&... args) {
	return TrackUtil::createTrack<quat4f, 4>(type, frameCount, SGE_FORWARD(args)...);
}

}