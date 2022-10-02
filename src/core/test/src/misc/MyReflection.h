#pragma once
#include <sge_core/log/Log.h>

namespace sge {

#define MY_TYPEOF_TIBASEINIT_DEFINE(T, BASE) \
private: \
	using This = T; \
	using Base = BASE; \
	class TI_Base : public TIBaseInit<T, BASE> { \
	public: \
		TI_Base() : TIBaseInit<T, BASE>(#T) {} \
	}; \
public: \
	static const MyTypeInfo* s_getType(); \
	virtual const MyTypeInfo* getType() const override { return s_getType(); } \
private: \
// ------------

#define MY_TYPEOF_TIBASEINITNOBASE_DEFINE(T) \
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

class MyTypeInfo;

class MyFieldInfo {
public:
	const char* name = "";
	const MyTypeInfo* fieldInfo = nullptr;
	intptr_t offset = 0;

	template<class Obj, class Member>
	MyFieldInfo(const char* name_, Member Obj::*ptr)
		: name(name_)
	{
		fieldInfo = my_typeof<Member>();
		offset = memberOffset(ptr);
	}

	void onFormat(fmt::format_context& ctx) const;
};

template<class T> const MyTypeInfo* my_typeof();

class MyTypeInfo {
public:
	const char* name = "";
	const MyTypeInfo* base = nullptr;
	size_t dataSize = 0;

	Span<MyFieldInfo> fieldArray;

	const MyTypeInfo* element = nullptr;

	bool isKindOf(const MyTypeInfo* target) const {
		if (!target) return nullptr;
		const MyTypeInfo* p = this;
		while(p) {
			if (p == target) return true;
			p = p->base;
		}
		return false;
	}

	template<class DST> inline
	bool isKindOf() const {
		return isKindOf(my_typeof<DST>());
	};

	void onFormat(fmt::format_context& ctx) const;
};

template<class T>
class TIBaseInitNoBase : public MyTypeInfo {
public:
	TIBaseInitNoBase(const char* name_) {
		name = name_;
		dataSize = sizeof(T);
	}
};

template<class T, class BASE>
class TIBaseInit : public TIBaseInitNoBase<T> {
public:
	TIBaseInit(const char* name_) : TIBaseInitNoBase<T>(name_) {
		static_assert(std::is_base_of<BASE, T>::value, "invalid base class");
		base = my_typeof<BASE>();
	}
};

template<class T, class ELEMENT>
class TIElementInit : public TIBaseInitNoBase<T> {
public:
	TIElementInit(const char* name_) : TIBaseInitNoBase<T>(name_) {
		element = my_typeof<ELEMENT>();
	}
};

class MyObject {
	MY_TYPEOF_TIBASEINITNOBASE_DEFINE(MyObject);
};

inline
const MyTypeInfo* my_typeof(MyObject& obj) {
	return obj.getType();
}

template<class DST>
inline DST* my_cast(MyObject* obj) {
	if (!obj) return nullptr;
	const auto* ti = my_typeof(*obj);
	if (!ti) return nullptr;
	if (!ti->isKindOf<DST>()) return nullptr;
	return static_cast<DST*>(obj);
};

#define my_typeof_define(T) template<> const MyTypeInfo* my_typeof<T>();

my_typeof_define(u8)
my_typeof_define(u16)
my_typeof_define(u32)
my_typeof_define(u64)

my_typeof_define(i8)
my_typeof_define(i16)
my_typeof_define(i32)
my_typeof_define(i64)

my_typeof_define(f32)
my_typeof_define(f64)
my_typeof_define(f128)

// String
my_typeof_define(TempStringA)
my_typeof_define(TempStringW)
my_typeof_define(StringA)
my_typeof_define(StringW)

// Vector
my_typeof_define(Vector<u8>)
my_typeof_define(Vector<u16>)
my_typeof_define(Vector<u32>)
my_typeof_define(Vector<u64>)

my_typeof_define(Vector<i8>)
my_typeof_define(Vector<i16>)
my_typeof_define(Vector<i32>)
my_typeof_define(Vector<i64>)

my_typeof_define(Vector<f32>)
my_typeof_define(Vector<f64>)
my_typeof_define(Vector<f128>)

// StringMap
my_typeof_define(StringMap<u8>)
my_typeof_define(StringMap<u16>)
my_typeof_define(StringMap<u32>)
my_typeof_define(StringMap<u64>)

my_typeof_define(StringMap<i8>)
my_typeof_define(StringMap<i16>)
my_typeof_define(StringMap<i32>)
my_typeof_define(StringMap<i64>)

my_typeof_define(StringMap<f32>)
my_typeof_define(StringMap<f64>)
my_typeof_define(StringMap<f128>)

SGE_FORMATTER(MyTypeInfo)
SGE_FORMATTER(MyFieldInfo)

template<class T> inline
const MyTypeInfo* my_typeof<T>() {
	return T::s_getType();
}

} // namespace