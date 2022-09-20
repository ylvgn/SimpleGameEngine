#include "MyReflection.h"

namespace sge {

#define my_typeof_pod_impl(T) \
template<> const MyTypeInfo* my_typeof<T>() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T) {} \
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

#define my_typeof_impl_s_getType(T) \
const MyTypeInfo* T::s_getType() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T) {} \
	}; \
	static TI ti; \
	return &ti; \
} \
// --------
my_typeof_impl_s_getType(MyObject)

void MyFieldInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, offset={}, type={}", name, offset, fieldInfo->name);
}

void MyTypeInfo::onFormat(fmt::format_context& ctx) const {
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