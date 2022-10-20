#pragma once

#include "Component.h"

namespace sge {

class CTransform : public Component {
	SGE_OBJECT_TYPE(CTransform, Component)
public:

	void setPosition(const Vec3f& pos) { _setLocalMatrixDirty(true);  _position.set(pos); }
	void setPosition(const float x, const float y, const float z) { _setLocalMatrixDirty(true); _position.set(x, y, z); }

	      Vec3f  getPosition()		 { return _position; }
	const Vec3f& getPosition() const { return _position; }

	void setRotate(const float x, const float y, const float z, const float w) { _setLocalMatrixDirty(true);  _rotate.set(x, y, z, w); }
	void setRotate(const Quat4f& q) { _setLocalMatrixDirty(true); _rotate.set(q.x, q.y, q.z, q.w); }

	      Quat4f  getRotate()		{ return _rotate; }
	const Quat4f& getRotate() const { return _rotate; }

	void setScale(const Vec3f& s) { _setLocalMatrixDirty(true); _scale.set(s); }

	      Vec3f  getScale()       { return _scale; }
	const Vec3f& getScale() const { return _scale; }

	const Mat4f& localMatrix();
	const Mat4f& worldMatrix();

private:

	const bool isWorldMatrixDirty() const { return (_dirty >> 1) & 1; }
	const bool isLocalMatrixDirty() const { return (_dirty >> 0) & 1; }

	void _setWorldMatrixDirty(bool is);
	void _setLocalMatrixDirty(bool is);

	int _dirty = 2; // 00 01 10 11 --> dirty at first

	Vec3f	_position{ 0,0,0 };
	Quat4f	_rotate{ 0,0,0,0 };
	Vec3f	_scale{ 1,1,1 };

	Mat4f _localMatrix;
	Mat4f _worldMatrix;

};

} // namespace