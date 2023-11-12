#pragma once

#include "Track.h"
#include <sge_game_anime_prog/math/Transform.h>

namespace sge {
/*
	For any animated transform, you don't want to maintain separate vector and quaternion tracks;
	instead, you build a higher-level structure

	A transform track encapsulates three component tracks:
		- position track
		- rotation track
		- scale track
*/

// The template takes two arguments—
	// the type of vector track and the type of quaternion
	// use the same code and accommodate for FastTrack
template<typename VTRACK, typename QTRACK>
struct TransformTrackT {
public:
	using SampleRequest = Track_SampleRequest;

	float getStartTime() const;
	float getEndTime()	 const;

	// Not all of the component tracks in a transform track need to be valid.
		// For example, if only the position of a transform is animated,
		// the rotation and scale component tracks can be left as invalid.
	// valid means at least one component track is valid.
	inline bool isValid() const { return position.isValid() || rotation.isValid() || scale.isValid(); }

	Transform sample(const Transform& t, const SampleRequest& sr) const;

	inline void setId(u32 id)	{ _id = id; }
	inline u32	id() const		{ return _id; }

	VTRACK	position;
	QTRACK	rotation;
	VTRACK	scale;

private:
	u32		_id = 0; // jointId
};

using TransformTrack	 = TransformTrackT<VectorTrack, QuaternionTrack>;
using FastTransformTrack = TransformTrackT<FastVectorTrack, FastQuaternionTrack>;

struct TransformTrackUtil {
	TransformTrackUtil() = delete;

	static UPtr<FastTransformTrack> optimizeTransformTrack(const TransformTrack& src);
};

}