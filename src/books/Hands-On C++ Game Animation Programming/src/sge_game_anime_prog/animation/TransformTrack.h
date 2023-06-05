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

class TransformTrack : public NonCopyable {
public:
	using SampleRequest = Track_SampleRequest;

	inline void setId(u32 id) { _id = id; }

	inline float getStartTime() const { return Math::min(_position.getStartTime(), Math::min(_rotation.getStartTime(), _scale.getStartTime())); }
	inline float getEndTime()	const { return Math::max(_position.getEndTime(), Math::max(_rotation.getEndTime(), _scale.getEndTime())); }

//	inline float saveGetStartTime() const { return Math::min(_position.saveGetStartTime(), Math::min(_rotation.saveGetStartTime(), _scale.saveGetStartTime())); }
//	inline float saveGetEndTime()	const { return Math::max(_position.saveGetEndTime(), Math::max(_rotation.saveGetEndTime(), _scale.saveGetEndTime())); }

	// valid means at least one component track is valid.
	inline bool isValid() const { return _position.isValid() || _rotation.isValid() || _scale.isValid(); }

	Transform sample(const Transform& t, const SampleRequest& sr);

	inline u32 id() const { return _id; }

private:
	u32 _id = 0; // boneId aka jointId

	// Not all of the component tracks in a transform track need to be valid.
	// For example, if only the position of a transform is animated, the rotationand scale component tracks can be left as invalid.
	VectorTrack		_position;
	QuaternionTrack _rotation;
	VectorTrack		_scale;
};

}