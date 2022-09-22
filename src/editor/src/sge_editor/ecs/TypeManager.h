#pragma once
#include "TypeObject.h"

namespace sge {

class TypeManager : public NonCopyable {
public:
	TypeManager();
	static TypeManager* instance();

	inline
	const TypeInfo* registerType(const TypeInfo* ti) {
		SGE_ASSERT(ti != nullptr);
		SGE_ASSERT(getType(ti->name) == nullptr);
		_table[ti->name] = ti;
		return ti;
	}

	template<class T> inline
	const TypeInfo* registerType() {
		const TypeInfo* ti = sge_typeof<T>();
		return registerType(ti);
	}

	const TypeInfo* getType(StrView typeName);
private:
	StringMap<const TypeInfo*> _table;
};

} // namespace