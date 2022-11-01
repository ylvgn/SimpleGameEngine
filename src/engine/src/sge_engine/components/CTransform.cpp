#include "CTransform.h"

#include "../Entity.h"

namespace sge {

const TypeInfo* CTransform::s_getType() {
	using This = CTransform;
	class Ti : public CTransform::TI_Base {
	public:
		static const Vec3f& getLocalPos(const void* obj) {
			const auto* c = reinterpret_cast<const CTransform*>(obj);
			return c->localPos();
		}

		static void setLocalPos(void* obj, const void* value) {
			auto* c = reinterpret_cast<CTransform*>(obj);
			auto& v = *reinterpret_cast<const Vec3f*>(value);
			c->setLocalPos(v);
		}

		static const Quat4f& getLocalRotate(const void* obj) {
			const auto* c = reinterpret_cast<const CTransform*>(obj);
			return c->localRotate();
		}

		static void setLocalRotate(void* obj, const void* value) {
			auto* c = reinterpret_cast<CTransform*>(obj);
			auto& v = *reinterpret_cast<const Quat4f*>(value);
			c->setLocalRotate(v);
		}

		static const Vec3f& getLocalScale(const void* obj) {
			const auto* c = reinterpret_cast<const CTransform*>(obj);
			return c->localScale();
		}

		static void setLocalScale(void* obj, const void* value) {
			auto* c = reinterpret_cast<CTransform*>(obj);
			auto& v = *reinterpret_cast<const Vec3f*>(value);
			c->setLocalScale(v);
		}

		Ti() {
			static FieldInfo fi[] = {
				FieldInfo("position", &This::_localPos,		&getLocalPos,		&setLocalPos),
				FieldInfo("rotate",   &This::_localRotate,	&getLocalRotate,	&setLocalRotate),
				FieldInfo("scale",    &This::_localScale,	&getLocalScale,		&setLocalScale),
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