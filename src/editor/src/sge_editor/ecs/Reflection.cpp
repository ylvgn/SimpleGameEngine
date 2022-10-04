#include "Reflection.h"

namespace sge {

#define sge_typeof_primitive_impl(T) \
template<> const TypeInfo* sge_typeof<T>() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T, TypeInfo::Style::Primitive) {} \
		static constexpr const char* getTypeStr() { return #T; } \
	}; \
	static TI ti; \
	return &ti; \
} \
// -------

sge_typeof_primitive_impl(u8)
sge_typeof_primitive_impl(u16)
sge_typeof_primitive_impl(u32)
sge_typeof_primitive_impl(u64)

sge_typeof_primitive_impl(i8)
sge_typeof_primitive_impl(i16)
sge_typeof_primitive_impl(i32)
sge_typeof_primitive_impl(i64)

sge_typeof_primitive_impl(f32)
sge_typeof_primitive_impl(f64)
sge_typeof_primitive_impl(f128)


#define Vec3f_FieldInfo_LIST() \
	sge_field_info(x), \
	sge_field_info(y), \
	sge_field_info(z), \
// ------------
sge_typeof_struct_impl(Vec3f)

#define Quat4f_FieldInfo_LIST() \
	sge_field_info(x), \
	sge_field_info(y), \
	sge_field_info(z), \
	sge_field_info(w), \
// ------------
sge_typeof_struct_impl(Quat4f)


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