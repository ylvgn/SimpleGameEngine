#pragma once

#include "Reflection.h"

namespace sge {

#define SGE_TYPEOF_DEFINE(T, BASE) \
public: \
	using This = T; \
	using Base = BASE; \
	class TI_Base : public TIBaseInit<T, BASE> { \
	public: \
		TI_Base() : TIBaseInit<T, BASE>(#T, TypeInfo::Style::Object) {} \
	}; \
	static constexpr const char* getTypeStr() { return #T; } \
	virtual const TypeInfo* getType() const override { return TypeInfo_get<T>(); } \
private: \
// ------------

class Object;
template<> const TypeInfo* TypeInfo_get<Object>();

class Object : public RefCountBase {
public:
	virtual ~Object() = default;
	virtual const TypeInfo* getType() const { return TypeInfo_get<Object>(); }
};

template<class DST> inline
DST* sge_cast(Object* obj) {
	if (!obj) return nullptr;
	const auto* ti = TypeManager::instance()->getType(DST::getTypeStr());
	if (!ti) {
		ti = TypeInfo_get<DST>();
		for (auto p : ti->fieldArray) {
			if (!TypeManager::instance()->getType(p.fieldInfo->name)) {
				TypeManager::instance()->registerType(p.fieldInfo);
			}
		}
	}
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

} // namespace