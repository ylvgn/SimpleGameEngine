#pragma once
#include "Reflection.h"

namespace sge {

#define SGE_TYPEOF_DEFINE(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
	class TI_Base : public TIBaseInit<T, BASE> { \
	public: \
		TI_Base() : TIBaseInit<T, BASE>(#T) {} \
	}; \
public: \
	static constexpr const char* getTypeStr() { return #T; } \
	static const TypeInfo* s_getType(); \
	virtual const TypeInfo* getType() const override { return s_getType(); } \
private: \
// ------------

#define SGE_TYPEOF_NOBASE_DEFINE(T) \
private: \
	using This = T; \
	class TI_Base : public TIBaseInitNoBase<T> { \
	public: \
		TI_Base() : TIBaseInitNoBase<T>(#T) {} \
	}; \
public: \
	static const MyTypeInfo* s_getType(); \
	virtual const MyTypeInfo* getType() const { return s_getType(); } \
private: \
// ------------

#define SGE_TYPE_IMPL_s_getType(T) \
const TypeInfo* T::s_getType() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T) {} \
	}; \
	static TI ti; \
	return &ti; \
} \
// --------

class TypeObject : public Object {
	using This = TypeObject;
	class TI_Base : public TIBaseInitNoBase<This> {
	public:
		TI_Base() : TIBaseInitNoBase<This>("TypeObject") {}
	};
public:
	static constexpr const char* getTypeStr() { return "TypeObject"; }
	static const TypeInfo* s_getType();
	virtual const TypeInfo* getType() const { return s_getType(); }
};

inline
const TypeInfo* sge_typeof(TypeObject& obj) {
	return obj.getType();
}

template<class DST> inline
DST* sge_cast(TypeObject* obj) {
	if (!obj) return nullptr;
	const auto* ti = TypeManager::instance()->getType(DST::getTypeStr());
	if (!ti) {
		ti = sge_typeof(*obj);
		if (ti) {
			TypeManager::instance()->registerType(ti);
		}
	}
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

} // namespace
