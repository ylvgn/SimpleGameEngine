#pragma once

#include "TransformTrack.h"
#include "Pose.h"

/*
	An animation clip is a collection of animation tracks.

	If you sample an animation clip, you get a pose that describes the configuration of each joint in the animation clip at the specified time.
	For a basic clip class, all you need is a vector of transform tracks.

	The Clip class implemented here can be used to animate anything; don't feel like you are limited to humans and humanoid animations.

	An animation clip always modifies the same joints.
	There is no need to re-set the pose that is sampled into so that it is the bind pose every frame.
*/

namespace sge {

template<typename TRACK>
class TClip {
public:
	inline TClip() :
		_name(""),
		_startTime(0.f),
		_endTime(0.f),
		_isLoop(true) { }

	// a public helper function to figure out the start and end times of the animation clip.
	// This function is intended to be called by the code that loads the animation clip from a file format.
	void recalculateDuration();

	// The sample function takes a Pose reference and a time and returns a float value that is also a time.
	float sample(Pose& out, float time) const;

	// []operator is meant to retrieve the TransformTrack object for a specific joint in the clip.
	TRACK& operator[](u32 jointId);

	inline float	getStartTime()  const  { return _startTime; }
	inline float	getEndTime()	const  { return _endTime; }
	inline float	getDuration()	const  { return _endTime - _startTime; }

	inline bool		isLoop()		const  { return _isLoop; };
	inline void		setIsLoop(bool isLoop) { _isLoop = isLoop; }

	inline StrView	name()			const  { return _name; }
	inline void		setName(StrView name)  { _name = name; }

	inline void setJointIdAtIndex(int i, u32 jointId) { _tracks[i].setId(jointId); }
	inline u32 getJointIdAtIndex(int i) const { return _tracks[i].id(); }

	inline size_t size() const { return _tracks.size(); }
	inline const Span<const TRACK> tracks() const { return _tracks; }
	inline void appendTrack(const TRACK& t) { _tracks.push_back(t); }

private:
	float _adjustTimeToFitRange(float time) const;

	Vector<TRACK>	_tracks;
	String			_name;
	float			_startTime;
	float			_endTime;
	bool			_isLoop;
};

using Clip		= TClip<TransformTrack>;
using FastClip	= TClip<FastTransformTrack>;

struct ClipUtil {
	ClipUtil() = delete;

	static FastClip optimizeClip(const Clip& src);
};

}