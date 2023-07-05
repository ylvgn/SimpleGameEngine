#pragma once

#include <sge_game_anime_prog/math/Transform.h>
#include "Track.h"

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
class TransformTrackT : public NonCopyable {
public:
	using SampleRequest = Track_SampleRequest;

	inline float getStartTime() const { return Math::min(_position.getStartTime(), Math::min(_rotation.getStartTime(), _scale.getStartTime())); }
	inline float getEndTime()	const { return Math::max(_position.getEndTime(), Math::max(_rotation.getEndTime(), _scale.getEndTime())); }
//	inline float saveGetStartTime() const { return Math::min(_position.saveGetStartTime(), Math::min(_rotation.saveGetStartTime(), _scale.saveGetStartTime())); }
//	inline float saveGetEndTime()	const { return Math::max(_position.saveGetEndTime(), Math::max(_rotation.saveGetEndTime(), _scale.saveGetEndTime())); }

	// valid means at least one component track is valid.
	inline bool isValid() const { return _position.isValid() || _rotation.isValid() || _scale.isValid(); }
//	inline bool isValid2() const { return _position.isValid2() || _rotation.isValid2() || _scale.isValid2(); }

	Transform sample(const Transform& t, const SampleRequest& sr) const;

	inline u32 id()				const { return _id; }
	const VTRACK& position()	const { return _position; }
	const QTRACK& rotation()	const { return _rotation; }
	const VTRACK& scale()		const { return _scale; }

	inline void setId(u32 id)				 { _id = id; }
	inline void setPosition(const VTRACK& p) { _position = p; }
	inline void setRotation(const QTRACK& r) { _rotation = r; }
	inline void setScale(const VTRACK& s)	 { _scale = s; }

private:
	u32 _id = 0; // jointId

	// Not all of the component tracks in a transform track need to be valid.
	// For example, if only the position of a transform is animated,
	// the rotation and scale component tracks can be left as invalid.
	VTRACK	_position;
	QTRACK	_rotation;
	VTRACK	_scale;
};

using TransformTrack	 = TransformTrackT<VectorTrack, QuaternionTrack>;
using FastTransformTrack = TransformTrackT<FastVectorTrack, FastQuaternionTrack>;

struct TransformTrackUtil {
	TransformTrackUtil() = delete;

	static UPtr<FastTransformTrack> optimizeTransformTrack(const TransformTrack& src);
};


}