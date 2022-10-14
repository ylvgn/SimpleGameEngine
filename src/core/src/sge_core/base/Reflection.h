#pragma once
#include "sge_base.h"
#include <sge_core/log/Log.h>

#define SGE_STRUCT_TYPE(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, TypeInfo::Style::Struct) {} \
	}; \
public: \
	static const TypeInfo* s_getType(); \
	virtual const TypeInfo* getType() const override { return s_getType(); } \
	static constexpr const char* s_getTypeName() { return #T; } \
//----

#define SGE_OBJECT_TYPE(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
	class TI_Base : public TypeInfoInit<T, BASE> { \
	public: \
		TI_Base() : TypeInfoInit<T, BASE>(#T, TypeInfo::Style::Object) {} \
	}; \
public: \
	static const TypeInfo* s_getType(); \
	virtual const TypeInfo* getType() const override { return s_getType(); } \
	static constexpr const char* s_getTypeName() { return #T; } \
private: \
//-----

#define SGE_STRUCT_TYPE_IMPL(T) \
template<> const TypeInfo* TypeOf<T>() { \
	class TI : public TypeInfoInitNoBase<T> { \
		using This = T; \
	public: \
		TI() : TypeInfoInitNoBase<T>(#T, TypeInfo::Style::Struct) { \
			static FieldInfo fi[] = { \
				T##_FieldInfo_LIST() \
			}; \
			setFieldInfo(fi); \
		} \
		static constexpr const char* s_getTypeName() { return #T; } \
	}; \
	static TI ti; \
	return &ti; \
} \
// ------------

#define SGE_FIELD_INFO(MEMBER) \
	FieldInfo(#MEMBER, &This::MEMBER) \
// ------------

namespace sge {

class Object;
class TypeInfo;

template<class T> inline const TypeInfo* TypeOf()			{ return T::s_getType(); }
template<class T> inline const TypeInfo* TypeOf(const T& v) { return TypeOf<T>(); }

class FieldInfo {
public:
	const char* name = "";
	const TypeInfo* fieldInfo = nullptr;
	intptr_t offset = 0;

	template<class OBJ, class FIELD>
	FieldInfo(const char* name_, FIELD OBJ::* ptr)
		: name(name_)
		, fieldInfo(TypeOf<FIELD>())
		, offset(memberOffset(ptr))
	{
	}

	void onFormat(fmt::format_context& ctx) const;
};

template<class T, class... ARGS> T* sge_creator(ARGS&&... args);
class TypeInfo {
public:
	const char* name = "";
	const TypeInfo* base = nullptr;
	size_t dataSize = 0;
	Span<FieldInfo> fieldArray;

	enum class Style {
		None = 0,
		Primitive,
		Struct,
		Object,
		Container,
	};
	Style style = Style::None;

	bool isKindOf(const TypeInfo* target) const {
		if (!target) return nullptr;
		const TypeInfo* p = this;
		while(p) {
			if (p == target) return true;
			p = p->base;
		}
		return false;
	}

	template<class DST> inline
	bool isKindOf() const {
		return isKindOf(TypeOf<DST>());
	};

	template<size_t N> inline
	void setFieldInfo(FieldInfo (&fi)[N]) {
		fieldArray = fi;
	}

	const bool isPrimitive()	const { return style == Style::Primitive; }
	const bool isStruct()		const { return style == Style::Struct; }
	const bool isObject()		const { return style == Style::Object; }
	const bool isContainer()	const { return style == Style::Container; }

	template<class DST, class... ARGS> inline
		DST* createObject(ARGS... args) const {
		SGE_ASSERT(isObject());
		return sge_creator<DST>(SGE_FORWARD(args)...);
	}

	template<class DST, class... ARGS> inline
	DST createPod(ARGS... args) const {
		SGE_ASSERT(isPrimitive() || isStruct());
		return DST(SGE_FORWARD(args)...);
	}

	void onFormat(fmt::format_context& ctx) const;
};

template<class T>
class TypeInfoInitNoBase : public TypeInfo {
public:
	TypeInfoInitNoBase(const char* name_, Style style_ = Style::Primitive) {
		name = name_;
		style = style_;
		dataSize = sizeof(T);
	}
};

template<class T, class BASE>
class TypeInfoInit : public TypeInfoInitNoBase<T> {
public:
	TypeInfoInit(const char* name_, Style style_) : TypeInfoInitNoBase<T>(name_, style_) {
		static_assert(std::is_base_of<BASE, T>::value, "invalid base class");
		base = TypeOf<BASE>();
	}
};

template<class T, class... ARGS> inline
T* sge_creator(ARGS&&... args) {
	return new T(SGE_FORWARD(args)...);
}

#if 0
template<class DST> inline
DST* sge_cast(Object* obj) {
	if (!obj) return nullptr;
	const auto* ti = TypeManager::instance()->getType(DST::s_getTypeName());
	if (!ti) {
		ti = TypeOf<DST>();
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
#endif

#define SGE_TYPEOF_SIMPLE(T) \
	template<> const TypeInfo* TypeOf<T>(); \
// -------

#define SGE_TYPEOF_SIMPLE_IMP(T) \
template<> const TypeInfo* TypeOf<T>() { \
	class TI : public TypeInfoInitNoBase<T> { \
	public: \
		TI() : TypeInfoInitNoBase<T>(#T, TypeInfo::Style::Primitive) {} \
		static constexpr const char* s_getTypeName() { return #T; } \
	}; \
	static TI ti; \
	return &ti; \
} \
// -------

SGE_TYPEOF_SIMPLE(float)
SGE_TYPEOF_SIMPLE(double)

SGE_TYPEOF_SIMPLE(int8_t)
SGE_TYPEOF_SIMPLE(int16_t)
SGE_TYPEOF_SIMPLE(int32_t)
SGE_TYPEOF_SIMPLE(int64_t)

SGE_TYPEOF_SIMPLE(uint8_t)
SGE_TYPEOF_SIMPLE(uint16_t)
SGE_TYPEOF_SIMPLE(uint32_t)
SGE_TYPEOF_SIMPLE(uint64_t)

SGE_TYPEOF_SIMPLE(char8_t) // c++20
SGE_TYPEOF_SIMPLE(char16_t)
SGE_TYPEOF_SIMPLE(char32_t)
SGE_TYPEOF_SIMPLE(wchar_t)

SGE_FORMATTER(TypeInfo)
SGE_FORMATTER(FieldInfo)
} // namespace