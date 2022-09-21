#include "TypeManager.h"
#include "Transform.h" //test

namespace sge {

const TypeInfo* TypeManager::getType(StrView typeName) {
	TempString tmpName = typeName;
	auto it = _table.find(tmpName.c_str());
	if (it != _table.end()) {
		return it->second;
	}
	return nullptr;
}

void TypeManager::Test() {
	sge_typeof<u8>();

	sge_typeof<u16>();
	sge_typeof<u32>();
	sge_typeof<u64>();
	sge_typeof<i8>();
	sge_typeof<i16>();
	sge_typeof<i32>();
	sge_typeof<i64>();
	sge_typeof<f32>();
	sge_typeof<f64>();
	sge_typeof<f128>();

	sge_typeof<Vec3f>();
	sge_typeof<Quat4f>();

	// just for test
	sge_typeof<Transform>();

	for (auto& s : _table) {
		SGE_DUMP_VAR(s.first, "==============");
		SGE_DUMP_VAR(*s.second);
	}
}


TypeManager* TypeManager::instance() {
	static TypeManager s;
	return &s;
}

} // namespace