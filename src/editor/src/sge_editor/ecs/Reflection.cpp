#include "Reflection.h"

namespace sge {

#define my_typeof_pod_impl(T) \
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
my_typeof_pod_impl(u8)
my_typeof_pod_impl(u16)
my_typeof_pod_impl(u32)
my_typeof_pod_impl(u64)

my_typeof_pod_impl(i8)
my_typeof_pod_impl(i16)
my_typeof_pod_impl(i32)
my_typeof_pod_impl(i64)

my_typeof_pod_impl(f32)
my_typeof_pod_impl(f64)
my_typeof_pod_impl(f128)

my_typeof_pod_impl(Vec2d)
my_typeof_pod_impl(Vec3d)
my_typeof_pod_impl(Vec4d)

my_typeof_pod_impl(Vec2f)
my_typeof_pod_impl(Vec3f)
my_typeof_pod_impl(Vec4f)

my_typeof_pod_impl(Quat4f)

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