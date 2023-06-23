#include "Pose.h"

namespace sge {

Pose& Pose::operator=(const Pose& r) {
	if (&r == this) {
		return *this;
	}

	_parentIds.resize(r._parentIds.size());
	_jointTrans.resize(r._jointTrans.size());

	if (_parentIds.size() != 0) {
		std::memcpy(_parentIds.data(),
			        r._parentIds.data(),
			        sizeof(int) * _parentIds.size()
		);
	}
	if (_jointTrans.size() != 0) {
		std::memcpy(_jointTrans.data(),
			        r._jointTrans.data(),
			        sizeof(Transform) * _jointTrans.size()
		);
	}

	SGE_ASSERT(_parentIds.size() == _jointTrans.size());
	return *this;
}

Transform Pose::getGlobalTransform(int i) const {
	Transform result = getLocalTransform(i);
	// Not all joints have parents;
	// if a joint doesn't have a parent, its parent value is negative.
	// negative means parent index < 0
	for (int p = _parentIds[i]; p >= 0; p = _parentIds[p]) {
		result = Transform::s_combine(_jointTrans[p], result);
	}
	return result;
}

void Pose::getMatrixPalette(Vector<mat4>& out) const {
	// For every transform, find the global transform, convert it into a matrix

	size_t jointCount = size();
	out.resize(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		Transform t = getGlobalTransform(i);
		out[i] = Transform::s_mat4(t);
	}
}

bool Pose::operator==(const Pose& r) const {
	if (_jointTrans.size() != r._jointTrans.size()) return false;
	if (_parentIds.size()  != r._parentIds.size())  return false;

	for (int i = 0; i < _parentIds.size(); ++i) {
		if (_parentIds[i] != r._parentIds[i]) return false;
	}
	for (int i = 0; i < _jointTrans.size(); ++i) {
		if (_jointTrans[i] != r._jointTrans[i]) return false;
	}

	return true;
}

bool Pose::operator!=(const Pose& r) const {
	return !this->operator==(r);
}

}