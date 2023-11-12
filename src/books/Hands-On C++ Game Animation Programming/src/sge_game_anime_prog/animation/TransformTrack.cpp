#include "TransformTrack.h"
#include "Track.h"

namespace sge {

template<typename VTRACK, typename QTRACK>
Transform TransformTrackT<VTRACK, QTRACK>::sample(const Transform& t, const SampleRequest& sr) const {
	Transform res = t;

	if (position.isValid2()) {
		res.position = position.sample(sr);
	}
	if (rotation.isValid2()) {
		res.rotation = rotation.sample(sr);
	}
	if (scale.isValid2()) {
		res.scale = scale.sample(sr);
	}

	return res;
}
template<typename VTRACK, typename QTRACK>
float TransformTrackT<VTRACK, QTRACK>::getStartTime() const {
	float result = 0.f;
	bool isSet = 0;

	if (position.getFrameCount() > 1) {
		float startTime = position.getStartTime();
		if (!isSet || startTime < result) {
			result = startTime;
			isSet = true;
		}
	}

	if (rotation.getFrameCount() > 1) {
		float startTime = rotation.getStartTime();
		if (!isSet || startTime < result) {
			result = startTime;
			isSet = true;
		}
	}

	if (scale.getFrameCount() > 1) {
		float startTime = scale.getStartTime();
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

	if (position.getFrameCount() > 1) {
		float endTime = position.getEndTime();
		if (!isSet || endTime > result) {
			result = endTime;
			isSet = true;
		}
	}

	if (rotation.getFrameCount() > 1) {
		float endTime = rotation.getEndTime();
		if (!isSet || endTime > result) {
			result = endTime;
			isSet = true;
		}
	}

	if (scale.getFrameCount() > 1) {
		float endTime = scale.getEndTime();
		if (!isSet || endTime > result) {
			result = endTime;
			isSet = true;
		}
	}

	return result;
}

template TransformTrackT<VectorTrack, QuaternionTrack>;
template TransformTrackT<FastVectorTrack, FastQuaternionTrack>;

#if 0
#pragma mark ================= TransformTrackUtil ====================
#endif
UPtr<FastTransformTrack> TransformTrackUtil::optimizeTransformTrack(const TransformTrack& src) {
	UPtr<FastTransformTrack> res(new FastTransformTrack());

	res->setId(src.id());

	res->position = TrackUtil::optimizeTrack(src.position);
	res->rotation = TrackUtil::optimizeTrack(src.rotation);
	res->scale    = TrackUtil::optimizeTrack(src.scale);

	return res;
}

}