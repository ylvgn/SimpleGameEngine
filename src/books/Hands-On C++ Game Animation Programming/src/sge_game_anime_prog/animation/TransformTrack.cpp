#include "TransformTrack.h"
#include "Track.h"

namespace sge {

template<typename VTRACK, typename QTRACK>
Transform TransformTrackT<VTRACK, QTRACK>::sample(const Transform& t, const SampleRequest& sr) const {
	Transform res = t;

	if (_position.isValid2()) {
		res.position = _position.sample(sr);
	}
	if (_rotation.isValid2()) {
		res.rotation = _rotation.sample(sr);
	}
	if (_scale.isValid2()) {
		res.scale = _scale.sample(sr);
	}

	return res;
}

template TransformTrackT<VectorTrack, QuaternionTrack>;
template TransformTrackT<FastVectorTrack, FastQuaternionTrack>;

UPtr<FastTransformTrack> TransformTrackUtil::optimizeTransformTrack(const TransformTrack& src) {
	UPtr<FastTransformTrack> res(new FastTransformTrack());
	res->setId(src.id());
	res->setPosition(TrackUtil::optimizeTrack(src.position()));
	res->setRotation(TrackUtil::optimizeTrack(src.rotation()));
	res->setScale(TrackUtil::optimizeTrack(src.scale()));
	return res;
}

}