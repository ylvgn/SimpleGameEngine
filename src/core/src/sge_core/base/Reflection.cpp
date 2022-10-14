#include "Reflection.h"

namespace sge {

SGE_TYPEOF_SIMPLE_IMP(u8)
SGE_TYPEOF_SIMPLE_IMP(u16)
SGE_TYPEOF_SIMPLE_IMP(u32)
SGE_TYPEOF_SIMPLE_IMP(u64)

SGE_TYPEOF_SIMPLE_IMP(i8)
SGE_TYPEOF_SIMPLE_IMP(i16)
SGE_TYPEOF_SIMPLE_IMP(i32)
SGE_TYPEOF_SIMPLE_IMP(i64)

SGE_TYPEOF_SIMPLE_IMP(f32)
SGE_TYPEOF_SIMPLE_IMP(f64)
SGE_TYPEOF_SIMPLE_IMP(f128)

//---------------------------------------------------------------------------------
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