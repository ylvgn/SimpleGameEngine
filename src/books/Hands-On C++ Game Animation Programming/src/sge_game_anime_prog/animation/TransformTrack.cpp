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
template<typename VTRACK, typename QTRACK>
float TransformTrackT<VTRACK, QTRACK>::getStartTime() const {
	float result = 0.f;
	bool isSet = 0;

	if (_position.getFrameCount() > 1) {
		float startTime = _position.getStartTime();
		if (!isSet || startTime < result) {
			result = startTime;
			isSet = true;
		}
	}

	if (_rotation.getFrameCount() > 1) {
		float startTime = _rotation.getStartTime();
		if (!isSet || startTime < result) {
			result = startTime;
			isSet = true;
		}
	}

	if (_scale.getFrameCount() > 1) {
		float startTime = _scale.getStartTime();
		if (!isSet || startTime < result) {
			result = startTime;
			isSet = true;
		}
	}

	return result;
}

template<typename VTRACK, typename QTRACK>
float TransformTrackT<VTRACK, QTRACK>::getEndTime() const {
	float result = 0.f;
	bool isSet = 0;

	if (_position.getFrameCount() > 1) {
		float endTime = _position.getEndTime();
		if (!isSet || endTime > result) {
			result = endTime;
			isSet = true;
		}
	}

	if (_rotation.getFrameCount() > 1) {
		float endTime = _rotation.getEndTime();
		if (!isSet || endTime > result) {
			result = endTime;
			isSet = true;
		}
	}

	if (_scale.getFrameCount() > 1) {
		float endTime = _scale.getEndTime();
		if (!isSet || endTime > result) {
			result = endTime;
			isSet = true;
		}
	}

	return result;
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