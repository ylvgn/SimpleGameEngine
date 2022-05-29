#pragma once

namespace sge {

enum class ShaderPropType {
	None,
	Int,
	Float,
	Vec2f,
	Vec3f,
	Vec4f,
	Color4f,
};

inline
bool enumTryParse(ShaderPropType& outValue, StrView str) {
#define E(V) if (str == #V) { outValue = ShaderPropType::V; return true; }
	E(None)
	E(Int)
	E(Float)
	E(Vec2f)
	E(Vec3f)
	E(Vec4f)
	E(Color4f)
#undef E
	return false;
}

struct ShaderInfo {

	struct Prop {
		ShaderPropType	propType = ShaderPropType::None;
		String			name;
		String			displayName;
		String			defaultValue;
	};

	struct Pass {
		String			name;
		String			vsFunc;
		String			psFunc;
	};

	Vector_<Prop, 8>	props;
	Vector_<Pass, 1>	passes;

	String name;
	void clean();

	void onFormat(fmt::format_context& ctx) const;

}; // ShaderInfo

inline
const char* enumStr(ShaderPropType v) {
	switch (v) {
#define E(T) case ShaderPropType::T: return #T;
		E(None)
		E(Int)
		E(Float)
		E(Vec2f)
		E(Vec3f)
		E(Vec4f)
		E(Color4f)
#undef E
	default: {
			SGE_ASSERT(false);
			return "";
		}
	}
}

} // namespace

SGE_FORMATTER(sge::ShaderInfo)
SGE_FORMATTER_ENUM(sge::ShaderPropType)