#pragma once
#include "Reflection.h"

namespace sge {

#define sge_type_info_get_struct_impl(T) \
template<> const TypeInfo* TypeInfo_get<T>() { \
	class TI : public TIBaseInitNoBase<T> { \
		using This = T; \
	public: \
		TI() : TIBaseInitNoBase<T>(#T, TypeInfo::Style::Struct) { \
			static FieldInfo fi[] = { \
				T##_FieldInfo_LIST() \
			}; \
			setFieldInfo(fi); \
		} \
		static constexpr const char* getTypeStr() { return #T; } \
	}; \
	static TI ti; \
	return &ti; \
} \
// ------------

#define sge_field_info(MEMBER) \
	FieldInfo(#MEMBER, &This::MEMBER) \
// ------------

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
		const TypeInfo* ti = TypeInfo_get<T>();
		return registerType(ti);
	}

	const TypeInfo* getType(StrView typeName);

private:
	StringMap<const TypeInfo*> _table;
};

} // namespace