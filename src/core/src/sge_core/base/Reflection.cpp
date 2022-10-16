#include "Reflection.h"

namespace sge {

SGE_TYPEOF_SIMPLE_IMP(float)
SGE_TYPEOF_SIMPLE_IMP(double)
SGE_TYPEOF_SIMPLE_IMP(long double)

SGE_TYPEOF_SIMPLE_IMP(int8_t)
SGE_TYPEOF_SIMPLE_IMP(int16_t)
SGE_TYPEOF_SIMPLE_IMP(int32_t)
SGE_TYPEOF_SIMPLE_IMP(int64_t)

SGE_TYPEOF_SIMPLE_IMP(uint8_t)
SGE_TYPEOF_SIMPLE_IMP(uint16_t)
SGE_TYPEOF_SIMPLE_IMP(uint32_t)
SGE_TYPEOF_SIMPLE_IMP(uint64_t)

SGE_TYPEOF_SIMPLE_IMP(char8_t) // c++20
SGE_TYPEOF_SIMPLE_IMP(char16_t)
SGE_TYPEOF_SIMPLE_IMP(char32_t)

SGE_TYPEOF_SIMPLE_IMP(wchar_t)

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