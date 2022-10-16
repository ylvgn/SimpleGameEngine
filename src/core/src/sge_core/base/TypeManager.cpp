#include "TypeManager.h"

namespace sge {

const TypeInfo* TypeManager::getType(StrView typeName) {
	TempString tmpName = typeName;
	auto it = _table.find(tmpName.c_str());
	if (it != _table.end()) {
		return it->second;
	}
	return nullptr;
}

TypeManager::TypeManager() {
	registerType<u8>();
	registerType<u16>();
	registerType<u32>();
	registerType<u64>();

	registerType<i8>();
	registerType<i16>();
	registerType<i32>();
	registerType<i64>();

	registerType<f32>();
	registerType<f64>();
	registerType<f128>();

	registerType<char8_t>(); // c++20
	registerType<char16_t>();
	registerType<char32_t>();
	registerType<wchar_t>();
}

TypeManager* TypeManager::instance() {
	static TypeManager s;
	return &s;
}

} // namespace