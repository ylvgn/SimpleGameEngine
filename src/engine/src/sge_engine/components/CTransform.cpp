#include "CTransform.h"

#include "../Entity.h"

namespace sge {

const TypeInfo* CTransform::s_getType() {
	class TI : public TI_Base {
	public:
		static void setLocalPos		(This& obj, const Vec3f&  v) { obj.setLocalPos(v); }
		static void setLocalRotate	(This& obj, const Quat4f& v) { obj.setLocalRotate(v); }
		static void setLocalScale	(This& obj, const Vec3f&  v) { obj.setLocalScale(v); }

		static const Vec3f&		getLocalPos		(const This& obj) { return obj.localPos(); }
		static const Quat4f&	getLocalRotate	(const This& obj) { return obj.localRotate(); }
		static const Vec3f&		getLocalScale	(const This& obj) { return obj.localScale(); }

		TI() {
			static FieldInfo fi[] = {
				{"position", &This::_localPos,		getLocalPos,	&setLocalPos},
				{"rotate",   &This::_localRotate,	getLocalRotate,	&setLocalRotate},
				{"scale",    &This::_localScale,	getLocalScale,	&setLocalScale},
			};
			setFields(fi);
		}
	};
	static TI ti;
	return &ti;
}

CTransform::CTransform() {
	_isRoot = false;
}

void CTransform::addChild(CTransform* c) {
	if (c->_parent) {
		c->_parent->removeChild(c);
	}

	c->_parent = _isRoot ? nullptr : this;
	c->_setWorldMatrixDirty();
	_children.emplace_back(c);
}

void CTransform::removeChild(CTransform* c) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if (*it == c) {
			_children.erase(it); // shoule be ordered
			c->_parent = nullptr;
			c->_setWorldMatrixDirty();
			return;
		}
	}
}

const Mat4f& CTransform::localMatrix() {
	if (_dirty.localMatrix) {
		_dirty.localMatrix = false;
		_computeLocalMatrix();
	}
	return _localMatrix;
}

const Mat4f& CTransform::worldMatrix() {
	if (_dirty.worldMatrix) {
		_dirty.worldMatrix = false;
		_computeWorldMatrix();
	}
	return _worldMatrix;
}

const Mat4f& CTransform::worldInvMatrix() {
	if (_dirty.worldInvMatrix) {
		_dirty.worldInvMatrix = false;
		_computeWorldInvMatrix();
	}
	return _worldInvMatrix;
}

void CTransform::_setLocalMatrixDirty() {
	if (_dirty.localMatrix) return;
	_dirty.localMatrix = true;
	_setWorldMatrixDirty();
}

void CTransform::_setWorldMatrixDirty() {
	if (_dirty.worldMatrix) return;
	_dirty.worldMatrix = true;
	_dirty.worldInvMatrix = true;

	for (auto& c : children()) {
		if (!c) continue;
		c->_setWorldMatrixDirty();
	}
}

void CTransform::_computeLocalMatrix() {
	_localMatrix = Mat4f::s_TRS(_localPos, _localRotate, _localScale);
}

void CTransform::_computeWorldMatrix() {
	if (_parent) {
		_worldMatrix = _parent->worldMatrix() * localMatrix();
	} else {
		_worldMatrix = localMatrix();
	}
}

void CTransform::_computeWorldInvMatrix() {
	_worldInvMatrix = worldMatrix().inverse();
}

void CTransform::setWorldPos(const Vec3f& pos) {
	if (!_parent) {
		setLocalPos(pos);
	} else {
		setLocalPos(_parent->worldInvMatrix().mulPoint4x3(pos));
	}
}

void CTransform::setWorldRotate(const Quat4f& q) {
	if (!_parent) {
		setLocalRotate(q);
	} else {
		setLocalRotate(_parent->worldRotate().inverse() * q);
	}
}

void CTransform::setWorldScale(const Vec3f& scale) {
	if (!_parent) {
		setLocalScale(scale);
	} else {
		setLocalScale(_parent->worldInvMatrix().mulPoint4x3(scale));
	}
}

Vec3f CTransform::worldPos() {
	return worldMatrix().col(3).toVec3();
}

Quat4f CTransform::worldRotate() {
	if (!_parent) {
		return _localRotate;
	} else {
		return _parent->worldRotate() * _localRotate;
	}
}

Vec3f CTransform::worldScale() {
	return worldMatrix().mulPoint4x3(_localScale);
}

} // namespace