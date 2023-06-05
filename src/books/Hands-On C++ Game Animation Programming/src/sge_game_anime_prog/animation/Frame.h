#pragma once

namespace sge {

// specialized frameand curve types, such as a scalar frame, a vector frame, and a quaternion frame
// Similarly, you could create separate classes to represent a scalar curve, a vector curve, and a quaternion curve
// for example, a vec3 track contains the Frame<3> type frames
// The Frame class you just implemented is used to store keyframes in an animation track
// If the interpolation type doesn't need tangents, you can simply ignore them

template<size_t N>
struct Frame {
	float value[N];
	float in[N];	// incoming tangent
	float out[N];	// outgoing tangent
	float time;
};

using ScalarFrame		= Frame<1>;
using VectorFrame		= Frame<3>;
using QuaternionFrame	= Frame<4>;

struct FrameUtil {
	FrameUtil() = delete;

	static ScalarFrame createFrame(float time, float in, float value, float out) {
		ScalarFrame result;
		result.time     = time;
		result.in[0]    = in;
		result.value[0] = value;
		result.out[0]   = out;
		return result;
	}

	static VectorFrame createFrame(float time, const vec3f& in, const vec3f& value, const vec3f& out) {
		VectorFrame result;
		result.time     = time;
		result.in[0]    = in.x;
		result.in[1]    = in.y;
		result.in[2]    = in.z;
		result.value[0] = value.x;
		result.value[1] = value.y;
		result.value[2] = value.z;
		result.out[0]   = out.x;
		result.out[1]   = out.y;
		result.out[2]   = out.z;
		return result;
	}

	static QuaternionFrame createFrame(float time, const quat& in, const quat& out, const quat& value) {
		QuaternionFrame result;
		result.time     = time;
		result.in[0]    = in.x;
		result.in[1]    = in.y;
		result.in[2]    = in.z;
		result.in[3]    = in.w;
		result.value[0] = value.x;
		result.value[1] = value.y;
		result.value[2] = value.z;
		result.value[3] = value.w;
		result.out[0]   = out.x;
		result.out[1]   = out.y;
		result.out[2]   = out.z;
		result.out[3]   = out.w;
		return result;
	}

	static ScalarFrame createFrame(float time, float value) {
		return createFrame(time, 0, value, 0);
	}

	static VectorFrame createFrame(float time, const vec3f& value) {
		return createFrame(time, vec3f::s_zero(), value, vec3f::s_zero());
	}

	static QuaternionFrame createFrame(float time, const quat& value) {
		return createFrame(time, quat::s_zero(), value, quat::s_zero());
	}

};

}