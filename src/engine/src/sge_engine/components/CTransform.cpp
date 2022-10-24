#include "CTransform.h"

#include "../Entity.h"

namespace sge {

const TypeInfo* CTransform::s_getType() {
	using This = CTransform;
	class Ti : public CTransform::TI_Base {
	public:
		static const Vec3f& getPosition(const void* obj) {
			const auto* c = reinterpret_cast<const CTransform*>(obj);
			return c->getPosition();
		}

		static void setPosition(void* obj, const void* value) {
			auto* c = reinterpret_cast<CTransform*>(obj);
			auto& v = *reinterpret_cast<const Vec3f*>(value);
			c->setPosition(v);
		}

		static const Quat4f& getRotate(const void* obj) {
			const auto* c = reinterpret_cast<const CTransform*>(obj);
			return c->getRotate();
		}

		static void setRotate(void* obj, const void* value) {
			auto* c = reinterpret_cast<CTransform*>(obj);
			auto& v = *reinterpret_cast<const Quat4f*>(value);
			c->setRotate(v.x, v.y, v.z, v.w);
		}

		static const Vec3f& getScale(const void* obj) {
			const auto* c = reinterpret_cast<const CTransform*>(obj);
			return c->getScale();
		}

		static void setScale(void* obj, const void* value) {
			auto* c = reinterpret_cast<CTransform*>(obj);
			auto& v = *reinterpret_cast<const Vec3f*>(value);
			c->setScale(v);
		}

		Ti() {
			static FieldInfo fi[] = {
				FieldInfo("position", &getPosition, &setPosition),
				FieldInfo("rotate",   &getRotate,   &setRotate),
				FieldInfo("scale",    &getScale,    &setScale),
			};
			setFields(fi);
		}
	};
	static Ti ti;
	return &ti;
}

const Mat4f& CTransform::localMatrix() {
	if (isLocalMatrixDirty()) {
		Mat4f T = Mat4f::s_translate(_position);

		const auto& q = _rotate;
		float x2 = q.x * q.x;
		float y2 = q.y * q.y;
		float z2 = q.z * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;
		Mat4f R = Mat4f({ 1 - 2 * (y2 + z2), 2 * (xy + wz),     2 * (xz - wy),     0 },
						{ 2 * (xy - wz),     1 - 2 * (x2 + z2), 2 * (yz + wx),     0 },
						{ 2 * (xz + wy),     2 * (yz - wx),     1 - 2 * (x2 + y2), 0 },
						{ 0,                 0,                 0,                 1 });

		Mat4f S = Mat4f::s_scale(_scale);

		_localMatrix = T * R * S;
		_setLocalMatrixDirty(false);
	}
	return _localMatrix;
}

const Mat4f& CTransform::worldMatrix() {
	auto* p = _entity->parent();
	if (isWorldMatrixDirty()) {
		if (!p) {
			_worldMatrix = localMatrix();
		} else {
			_worldMatrix = p->transform()->worldMatrix() * localMatrix();
		}
		_setWorldMatrixDirty(false);
	}
	return _worldMatrix;
}

void CTransform::_setWorldMatrixDirty(bool is) {
	if (!is) {
		_dirty &= ~(MatrixDirty::World);
		return;
	}

	_dirty |= MatrixDirty::World;
	for (auto& c : _entity->children()) {
		auto* t = c->transform();
		if (!t->isWorldMatrixDirty()) {
			t->_setWorldMatrixDirty(true);
		}
	}
}

void CTransform::_setLocalMatrixDirty(bool is) {
	if (!is) {
		_dirty &= ~(MatrixDirty::Local);
		return;
	}
	_dirty |= MatrixDirty::Local;
	_setWorldMatrixDirty(true);
}

} // namespace