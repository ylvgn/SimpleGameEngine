#pragma once

#include "Component.h"

namespace sge {

class CTransform : public Component {
	SGE_OBJECT_TYPE(CTransform, Component)
public:

	void setPosition(const Vec3f& pos) { position.set(pos); }

	      Vec3f  getPosition()		 { return position; }
	const Vec3f& getPosition() const { return position; }

	void setRotate(const float x, const float y, const float z, const float w) { rotate.set(x, y, z, w); }
	void setRotate(const Quat4f& q) { rotate.set(q.x, q.y, q.z, q.w); }

	      Quat4f  getRotate()		{ return rotate; }
	const Quat4f& getRotate() const { return rotate; }

	void setScale(const Vec3f& s) { scale.set(s); }

	      Vec3f  getScale()       { return scale; }
	const Vec3f& getScale() const { return scale; }

public:
	Vec3f	position{ 0,0,0 };
	Quat4f	rotate{ 0,0,0,0 };
	Vec3f	scale{ 1,1,1 };

	void onFormat(fmt::format_context& ctx) const;
};

SGE_FORMATTER(CTransform)

} // namespace