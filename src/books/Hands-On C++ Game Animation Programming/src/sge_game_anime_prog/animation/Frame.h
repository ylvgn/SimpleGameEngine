#pragma once

namespace sge {

/*
	specialized frameand curve types, such as a scalar frame, a vector frame, and a quaternion frame
	Similarly, you could create separate classes to represent a scalar curve, a vector curve, and a quaternion curve
	for example, a vec3 track contains the Frame<3> type frames
	The Frame class you just implemented is used to store keyframes in an animation track
	If the interpolation type doesn't need tangents, you can simply ignore them
*/

template<size_t N>
struct Frame {
	float value[N];
	float in[N];	// in-coming tangent
	float out[N];	// out-going tangent
	float time;
};

using ScalarFrame		= Frame<1>;
using VectorFrame		= Frame<3>;
using QuaternionFrame	= Frame<4>;

struct FrameUtil {
	FrameUtil() = delete;

	static ScalarFrame createFrame(float time, float in, float value, float out) {
		ScalarFrame res;
		res.time     = time;
		res.in[0]    = in;
		res.value[0] = value;
		res.out[0]   = out;
		return res;
	}

	static VectorFrame createFrame(float time, const vec3f& in, const vec3f& value, const vec3f& out) {
		VectorFrame res;
		res.time     = time;
		res.in[0]    = in.x;
		res.in[1]    = in.y;
		res.in[2]    = in.z;
		res.value[0] = value.x;
		res.value[1] = value.y;
		res.value[2] = value.z;
		res.out[0]   = out.x;
		res.out[1]   = out.y;
		res.out[2]   = out.z;
		return res;
	}

	static QuaternionFrame createFrame(float time, const quat4f& in, const quat4f& out, const quat4f& value) {
		QuaternionFrame res;
		res.time     = time;
		res.in[0]    = in.x;
		res.in[1]    = in.y;
		res.in[2]    = in.z;
		res.in[3]    = in.w;
		res.value[0] = value.x;
		res.value[1] = value.y;
		res.value[2] = value.z;
		res.value[3] = value.w;
		res.out[0]   = out.x;
		res.out[1]   = out.y;
		res.out[2]   = out.z;
		res.out[3]   = out.w;
		return res;
	}

	static ScalarFrame createFrame(float time, float value) {
		return createFrame(time, 0, value, 0);
	}

	static VectorFrame createFrame(float time, const vec3f& value) {
		return createFrame(time, vec3f::s_zero(), value, vec3f::s_zero());
	}

	static QuaternionFrame createFrame(float time, const quat4f& value) {
		return createFrame(time, quat4f::s_zero(), value, quat4f::s_zero());
	}
};

}