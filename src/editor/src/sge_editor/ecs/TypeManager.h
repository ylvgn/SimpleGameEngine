#pragma once
#include "Reflection.h"

namespace sge {

class TypeManager : public NonCopyable {
public:
	static TypeManager* instance();

	void Test();

	void registerType(const TypeInfo* ti) {
		auto* p = getType(ti->name);
		if (p) return;
		_table[ti->name] = ti;
	}

	const TypeInfo* getType(StrView typeName);

private:
	StringMap<const TypeInfo*> _table;
};

} // namespace