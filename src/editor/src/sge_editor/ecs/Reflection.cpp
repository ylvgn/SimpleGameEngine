#include "Reflection.h"

namespace sge {

#define sge_typeof_pod_impl(T) \
template<> const TypeInfo* sge_typeof<T>() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T) {} \
		static constexpr const char* getTypeStr() { return #T; } \
	}; \
	static TI ti; \
	return &ti; \
} \
// -------

sge_typeof_pod_impl(u8)
sge_typeof_pod_impl(u16)
sge_typeof_pod_impl(u32)
sge_typeof_pod_impl(u64)

sge_typeof_pod_impl(i8)
sge_typeof_pod_impl(i16)
sge_typeof_pod_impl(i32)
sge_typeof_pod_impl(i64)

sge_typeof_pod_impl(f32)
sge_typeof_pod_impl(f64)
sge_typeof_pod_impl(f128)

template<>
const TypeInfo* sge_typeof<Vec3f>() {
	class TI : public TIBaseInitNoBase<Vec3f> {
	public:
		TI() : TIBaseInitNoBase<Vec3f>("Vec3f", TypeInfo::Style::Struct) {
			static FieldInfo fi[] = {
				FieldInfo("x",	&Vec3f::x),
				FieldInfo("y",	&Vec3f::y),
				FieldInfo("z",	&Vec3f::z),
			};
			setFieldInfo(fi);
		}
		static constexpr const char* getTypeStr() { return "Vec3f"; }
	};
	static TI ti;
	return &ti;
}
// ------- Vec3f

template<>
const TypeInfo* sge_typeof<Quat4f>() {
	class TI : public TIBaseInitNoBase<Quat4f> {
	public:
		TI() : TIBaseInitNoBase<Quat4f>("Quat4f", TypeInfo::Style::Struct) {
			static FieldInfo fi[] = {
				FieldInfo("x",	&Quat4f::x),
				FieldInfo("y",	&Quat4f::y),
				FieldInfo("z",	&Quat4f::z),
				FieldInfo("w",	&Quat4f::w),
			};
			setFieldInfo(fi);
		}
		static constexpr const char* getTypeStr() { return "Quat4f"; }
	};
	static TI ti;
	return &ti;
}
// ------- Quat4f

void FieldInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, offset={}, type={}", name, offset, fieldInfo->name);
}

void TypeInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, dataSize={}", name, dataSize);
	if (base) {
		fmt::format_to(ctx.out(), ", base=[{}]", *base);
	}
	if (!fieldArray.empty()) fmt::format_to(ctx.out(), "{}", ", fields={");
	for (auto& f : fieldArray) {
		fmt::format_to(ctx.out(), " [{}],", f);
	}
	if (!fieldArray.empty()) fmt::format_to(ctx.out(), " {}", '}');
}

} // namespace