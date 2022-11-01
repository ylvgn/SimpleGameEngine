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
			setFields(fi); \
		} \
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
	template<class T> using Gettor = const T& (*)(const void*);
	using Settor = void (*)(void*, const void*);

	template<class OBJ, class FIELD>
	FieldInfo(	const char* name_,
				FIELD OBJ::* ptr,
				Gettor<FIELD> getter_ = nullptr,
				Settor setter_ = nullptr)
		: name(name_)
		, fieldType(TypeOf<FIELD>())
		, offset(memberOffset(ptr))
		, getter(getter_)
		, setter(setter_)
	{
	}

		  void* getValuePtr(      void* obj) const { return reinterpret_cast<      u8*>(obj) + offset; }
	const void* getValuePtr(const void* obj) const { return reinterpret_cast<const u8*>(obj) + offset; }

	template<class T>
	const T& getValue(const void* obj) const {
		SGE_ASSERT(TypeOf<T>() == fieldType);
		if (getter) {
			return reinterpret_cast<Gettor<T>>(getter)(obj);
		} else {
			return *reinterpret_cast<const T*>(getValuePtr(obj));
		}
	}

	template<class T>
	void setValue(void* obj, const T& value) const {
		SGE_ASSERT(TypeOf<T>() == fieldType);
		if (setter) {
			setter(obj, &value);
		} else {
			*reinterpret_cast<T*>(getValuePtr(obj)) = value;
		}
	}

	void onFormat(fmt::format_context& ctx) const;

	const char* name			= "";
	const TypeInfo* fieldType	= nullptr;
	intptr_t offset				= 0;
	void* getter				= nullptr;
	Settor setter				= nullptr;
};

class TypeInfo {
public:
	const char* name = "";
	const TypeInfo* base = nullptr;
	size_t dataSize = 0;

	Span<const FieldInfo> fields() const { return _fields; }

	using Creator = Object * (*)();

	Creator creator;

	Object* createObject() const {
		if (!creator) return nullptr;
		return creator();
	}

	enum class Style {
		None = 0,
		Primitive,
		Struct,
		Object,
		Container,
	};
	Style style = Style::None;

	const bool isPrimitive()	const { return style == Style::Primitive; }
	const bool isStruct()		const { return style == Style::Struct; }
	const bool isObject()		const { return style == Style::Object; }
	const bool isContainer()	const { return style == Style::Container; }

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

	void onFormat(fmt::format_context& ctx) const;

protected:
	Span<const FieldInfo> _fields;
};

template<class T>
class TypeInfoInitNoBase : public TypeInfo {
public:
	TypeInfoInitNoBase(const char* name_, Style style_ = Style::Primitive) {
		name = name_;
		style = style_;
		dataSize = sizeof(T);
	}

	template<size_t N>
	void setFields(const FieldInfo(&fi)[N]) {
		_fields = fi;
	}
};

template<class T> inline
static Object* TypeCreator() {
	return new T();
}

template<class T, class BASE>
class TypeInfoInit : public TypeInfoInitNoBase<T> {
public:
	TypeInfoInit(const char* name, Style style_) : TypeInfoInitNoBase<T>(name, style_) {
		static_assert(std::is_base_of<BASE, T>::value, "invalid base class");
		base = TypeOf<BASE>();

		this->creator = &TypeCreator<T>;
	}
};

template<class DST> inline
DST* sge_cast(Object* obj) {
	if (!obj) return nullptr;
	const auto* ti = TypeOf<DST>();
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

#define SGE_TYPEOF_SIMPLE(T) \
	template<> const TypeInfo* TypeOf<T>(); \
// -------

#define SGE_TYPEOF_SIMPLE_IMP(T) \
template<> const TypeInfo* TypeOf<T>() { \
	class TI : public TypeInfoInitNoBase<T> { \
	public: \
		TI() : TypeInfoInitNoBase<T>(#T, TypeInfo::Style::Primitive) {} \
	}; \
	static TI ti; \
	return &ti; \
} \
// -------

SGE_TYPEOF_SIMPLE(float)
SGE_TYPEOF_SIMPLE(double)
SGE_TYPEOF_SIMPLE(long double)

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