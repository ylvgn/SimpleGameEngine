#include "Pose.h"

namespace sge {

Pose& Pose::operator=(const Pose& r) {
	if (&r == this) {
		return *this;
	}

	_parents.resize(r._parents.size());
	_joints.resize(r._joints.size());

	if (_parents.size() != 0) {
		std::memcpy(_parents.data(),
			        r._parents.data(),
			        sizeof(int) * _parents.size()
		);
	}
	if (_joints.size() != 0) {
		std::memcpy(_joints.data(),
			        r._joints.data(),
			        sizeof(Transform) * _joints.size()
		);
	}

	SGE_ASSERT(_parents.size() == _joints.size());
	return *this;
}

Transform Pose::getGlobalTransform(int i) const {
	Transform result = getLocalTransform(i);
	// Not all joints have parents;
	// if a joint doesn't have a parent, its parent value is negative.
	// negative means parent index < 0
	for (int p = _parents[i]; p >= 0; p = _parents[p]) {
		result = Transform::s_combine(_joints[p], result);
	}
	return result;
}

void Pose::getMatrixPalette(Vector<mat4>& out) const {
	// For every transform, find the global transform, convert it into a matrix

	size_t jointCount = size();
	out.resize(jointCount);

	for (int i = 0; i < jointCount; ++i) {
		out[i] = Transform::s_mat4(getGlobalTransform(i));
	}
}

bool Pose::operator==(const Pose& r) const {
	if (_joints.size()  != r._joints.size())  return false;
	if (_parents.size() != r._parents.size()) return false;

	for (int i = 0; i < _parents.size(); ++i) {
		if (_parents[i] != r._parents[i]) return false;
	}
	for (int i = 0; i < _joints.size(); ++i) {
		if (_joints[i] != r._joints[i]) return false;
	}

	return true;
}

bool Pose::operator!=(const Pose& r) const {
	return !this->operator==(r);
}

}