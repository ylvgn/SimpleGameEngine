#include "CTransform.h"

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
				FieldInfo("position", &This::position, &getPosition, &setPosition),
				FieldInfo("rotate",	&This::rotate, &getRotate, &setRotate),
				FieldInfo("scale", &This::scale, &getScale, &setScale),
			};
			setFields(fi);
		}
	};
	static Ti ti;
	return &ti;
}

void CTransform::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "position=({},{}, {}), rotate=({}, {}, {}, {}), scale=({}, {}, {})",
		position.x, position.y, position.z,
		rotate.x, rotate.y, rotate.z, rotate.w,
		scale.x, scale.y, scale.z
	);
}

} // namespace