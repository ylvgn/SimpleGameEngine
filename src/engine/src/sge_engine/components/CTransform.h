#pragma once

#include "Component.h"

namespace sge {

class CTransform : public Component {
	SGE_OBJECT_TYPE(CTransform, Component)
public:

	void setLocalPos	(float x, float y, float z) { setLocalPos(Vec3f(x,y,z)); }
	void setLocalPos	(const Vec3f&  v) { _localPos = v;		_setLocalMatrixDirty(); }

	void setLocalRotate	(const Quat4f& v) { _localRotate = v;	_setLocalMatrixDirty(); }
	void setLocalScale	(const Vec3f&  v) { _localScale = v;	_setLocalMatrixDirty(); }

	const Vec3f&	localPos()	const	{ return _localPos; }
	const Quat4f&	localRotate() const	{ return _localRotate; }
	const Vec3f&	localScale() const	{ return _localScale; }

	void setParent(CTransform* c);
	void addChild(CTransform* c);
	void removeChild(CTransform* c);

	Span<CTransform*> children()	{ return _children; }
	CTransform*	getChild(size_t i)	{ return (i >= 0 && i < _children.size()) ? _children[i] : nullptr; }
	size_t childCount() const		{ return _children.size(); }

	const Mat4f& localMatrix();
	const Mat4f& worldMatrix();

	void setWorldPos(const Vec3f& pos) {
		if (!_parent) {
			setLocalPos(pos);
		} else {
			setLocalPos(worldMatrix().inverse().mulPoint4x3(pos));
		}
	}
	const Vec3f worldPos() { return worldMatrix().col(3).toVec3(); }

private:

	void _computeLocalMatrix();
	void _computeWorldMatrix();

	void _setLocalMatrixDirty();
	void _setWorldMatrixDirty();

	Vec3f	_localPos{ 0,0,0 };
	Quat4f	_localRotate{ 0,0,0,0 };
	Vec3f	_localScale{ 1,1,1 };

	struct Dirty {
		Dirty()
			: localMatrix(true)
			, worldMatrix(true)
		{}
		bool	localMatrix : 1;
		bool	worldMatrix : 1;
	};

	Dirty		_dirty;
	CTransform*	_parent = nullptr;
	Vector<CTransform*>	_children;

	Mat4f	_localMatrix;
	Mat4f	_worldMatrix;
};

} // namespace