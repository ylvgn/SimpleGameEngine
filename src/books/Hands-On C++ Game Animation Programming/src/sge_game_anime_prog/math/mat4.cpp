#include "mat4.h"

#include "Transform.h"
#include "dual_quat.h"

namespace sge {

void mat4::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "mat4(\n  {},\n  {},\n  {},\n  {})", cx(), cy(), cz(), cw());
}

mat4 mat4::s_transform(const Transform& t) {
#if 0
	// First, extract the rotation basis of the transform
	vec3 r = t.rotation * vec3::s_right();
	vec3 u = t.rotation * vec3::s_up();
	vec3 f = t.rotation * vec3::s_forward();

	// Next, scale the basis vectors
	r = r * t.scale.x;
	u = u * t.scale.y;
	f = f * t.scale.z;

	return mat4(r.x,          r.y,          r.z,          0, // X basis (& scale)
				u.x,          u.y,          u.z,          0, // Y basis (& scale)
				f.x,          f.y,          f.z,          0, // Z basis (& scale)
				t.position.x, t.position.y, t.position.z, 1  // Position
	);
#else	
	return mat4::s_trs(t.position, t.rotation, t.scale);
#endif
}

mat4 mat4::s_dual_quat(const dual_quat& dq) {
	// dual-quaternions can be converted to and from matrices as well;
	// however, that operation 'mat4::s_dual_quat' is generally not used.

	// result = mat4::s_trs(dq.toTranslation(), dq.toRotation(), vec3f::s_one());

	auto n   = dq.normalize();
	auto q   = n.toRotation();
	auto t   = n.toTranslation();

	float xx = q.x*q.x;
	float yy = q.y*q.y;
	float zz = q.z*q.z;
	float wx = q.w*q.x;
	float wy = q.w*q.y;
	float wz = q.w*q.z;
	float xy = q.x*q.y;
	float xz = q.x*q.z;
	float yz = q.y*q.z;

	return mat4(1-2*(yy+zz),	2*(xy+wz),		2*(xz-wy),		0,
		        2*(xy-wz),      1-2*(xx+zz),	2*(yz+wx),		0,
		        2*(xz+wy),      2*(yz-wx),		1-2*(xx+yy),	0,
				t.x,			t.y,			t.z,			1);
}

}