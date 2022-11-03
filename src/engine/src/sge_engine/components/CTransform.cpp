#include "CTransform.h"

#include "../Entity.h"

namespace sge {

const TypeInfo* CTransform::s_getType() {
	class Ti : public CTransform::TI_Base {
	public:
		static void setLocalPos		(This& obj, const Vec3f&  v) { obj.setLocalPos(v); }
		static void setLocalRotate	(This& obj, const Quat4f& v) { obj.setLocalRotate(v); }
		static void setLocalScale	(This& obj, const Vec3f&  v) { obj.setLocalScale(v); }

		static const Vec3f&		getLocalPos		(const This& obj) { return obj.localPos(); }
		static const Quat4f&	getLocalRotate	(const This& obj) { return obj.localRotate(); }
		static const Vec3f&		getLocalScale	(const This& obj) { return obj.localScale(); }

		Ti() {
			static FieldInfo fi[] = {
				{"position", &This::_localPos,		getLocalPos,	&setLocalPos},
				{"rotate",   &This::_localRotate,	getLocalRotate,	&setLocalRotate},
				{"scale",    &This::_localScale,	getLocalScale,	&setLocalScale},
			};
			setFields(fi);
		}
	};
	static Ti ti;
	return &ti;
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

void CTransform::_setLocalMatrixDirty() {
	if (_dirty.localMatrix) return;
	_dirty.localMatrix = true;
	_setWorldMatrixDirty();
}

void CTransform::_setWorldMatrixDirty() {
	if (_dirty.worldMatrix) return;
	_dirty.worldMatrix = true;

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

void CTransform::setParent(CTransform* parent) {
	if (parent == this) return;
	if (_parent == parent) return;

	if (_parent) {
		_parent->removeChild(this);
	}

	_parent = parent;
	if (!parent) return;

	parent->addChild(this);
	setLocalPos(_localPos - _parent->localPos());
	setLocalRotate(_localRotate - _parent->localRotate());
	setLocalScale(_localScale / _parent->localScale());
}

void CTransform::addChild(CTransform* c) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if (*it == c) {
			return;
		}
	}

	_children.emplace_back(c);
	c->_parent = this;
}

void CTransform::removeChild(CTransform* c) {
	for (auto* it = _children.begin(); it != _children.end(); it++) {
		if (*it == c) {
			_children.erase(it); // shoule be ordered
			return;
		}
	}
}

} // namespace