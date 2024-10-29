#pragma once

#include "TransformTrack.h"
#include "Pose.h"

namespace sge {
/*
	An animation clip is a collection of animation tracks.

	If you sample an animation clip, you get a pose that describes the configuration of each joint in the animation clip at the specified time.
	For a basic clip class, all you need is a vector of transform tracks.

	The Clip class implemented here can be used to animate anything;
	don't feel like you are limited to humans and humanoid animations.

	An animation clip always modifies the same joints.
	There is no need to re-set the pose that is sampled into so that it is the bind pose every frame.
*/

template<typename TRANSFORMTRACK>
struct ClipT {
public:
	using TRACK			= TRANSFORMTRACK;
	using SampleRequest = Track_SampleRequest;

	ClipT()
		: _name("")
		, _startTime(0.f)
		, _endTime(0.f)
		, _isLoop(true)
	{}

	static UPtr<TRACK> s_createTrack(u32 jointId) {
		UPtr<TRACK> res = UPtr_make<TRACK>(); // do not use auto type, use explicit UPtr<TRACK>
		res->jointId = jointId;
		return res;
	}

	float sample(Pose& out, float time) const;

	SGE_INLINE float	getStartTime()  const	{ return _startTime; }
	SGE_INLINE float	getEndTime()	const	{ return _endTime; }
	SGE_INLINE float	getDuration()	const	{ return _endTime - _startTime; }

	SGE_INLINE bool		isLoop()		const	{ return _isLoop; }
	SGE_INLINE void		setIsLoop(bool isLoop)	{ _isLoop = isLoop; }

	SGE_INLINE StrView	name()			const	{ return _name; }
	SGE_INLINE void		setName(StrView name)	{ _name = name; }

	SGE_INLINE size_t	getTrackCount() const	{ return _tracks.size(); }

	TRACK* findTrackByJointId(u32 jointId);

	TRACK* getTrack_noCheck(size_t index) const { return _tracks[index].get(); }
	TRACK* getTrack(size_t index) const {
		if (index >= _tracks.size()) {
			SGE_ASSERT(false);
			return nullptr;
		}
		return getTrack_noCheck(index);
	}

	void reserve(size_t newTrackCount)			{ _tracks.reserve(newTrackCount); }
	void appendTrack(UPtr<TRACK> && t)			{ _tracks.emplace_back(eastl::move(t)); }

	void	recalculateDuration();
	TRACK&	getOrCreateTrackByJointId(u32 jointId);

private:
	float _adjustTimeToFitRange(float time) const;

	Vector< UPtr<TRACK> > _tracks;

	String			      _name;
	float			      _startTime;
	float			      _endTime;
	bool			      _isLoop;
};

using Clip		= ClipT<TransformTrack>;
using FastClip	= ClipT<FastTransformTrack>;

struct ClipUtil {
	ClipUtil() = delete;

	static FastClip optimizeClip(const Clip& src);
};

}