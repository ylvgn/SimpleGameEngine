#include "Pose.h"

namespace sge {

void Pose::resize(size_t newJointCount) {
	// Since the parent and joint vectors are parallel, the resize function needs to set the size of both.
	// at initialization, joint's parent is self, parentIds[i]==-1 means no parent
	_parentIds.resize(newJointCount);
	_jointTrans.resize(newJointCount);
}

void Pose::clear() {
	resize(0);
}

Pose& Pose::operator=(const Pose& r) {
	// overload operator=, use std::memcpy for quick copy

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

#if 1
void Pose::getMatrixPalette(Vector<mat4>& out) const {
	size_t jointCount = getJointCount();
	out.resize(jointCount);

	int i = 0;
	for (; i < jointCount; ++i) { // first loop
		int p = _parentIds[i];
		if (p > i) {
			// If the joint parent has a smaller index than the joint, the optimized method is used.
			// If the joint's parent isn't smaller, the first loop breaks out and gives the second loop a chance to run.
			break;
		}

		mat4 local = mat4::s_transform(_jointTrans[i]);
		if (p >= 0) { // maybe p == -1
			out[i] = out[p] * local;
		} else {
			out[i] = local;
		}
	}

	// fallback
	for (; i < jointCount; ++i) { // second loop
		Transform global = getGlobalTransform(i);
		out[i] = mat4::s_transform(global);
	}
}
#else
void Pose::getMatrixPalette(Vector<mat4>& out) const {
	// converts a pose into a linear array of matrices
	// For every transform, find the global transform, convert it into a matrix

	size_t jointCount = size();
	out.resize(jointCount);
	for (int i = 0; i < jointCount; ++i) {
		Transform global = getGlobalTransform(i);
		out[i] = mat4::s_transform(global);
	}
}
#endif

bool Pose::operator==(const Pose& r) const {
	if ((_jointTrans.size() != r._jointTrans.size())
	 || (_parentIds.size()  != r._parentIds.size()))
	{
		return false;
	}

	for (int i = 0; i < _parentIds.size(); ++i) {
		if (_parentIds[i] != r._parentIds[i])
			return false;
	}
	for (int i = 0; i < _jointTrans.size(); ++i) {
		if (_jointTrans[i] != r._jointTrans[i])
			return false;
	}

	return true;
}

bool Pose::operator!=(const Pose& r) const {
	return !this->operator==(r);
}

}