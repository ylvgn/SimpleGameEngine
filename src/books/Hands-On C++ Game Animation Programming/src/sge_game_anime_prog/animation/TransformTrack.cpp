#include "TransformTrack.h"

namespace sge {

Transform TransformTrack::sample(const Transform& t, const SampleRequest& sr) const {
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

}