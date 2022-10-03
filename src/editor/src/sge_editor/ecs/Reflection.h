#pragma once
#include <sge_core/log/Log.h>

namespace sge {

class TypeInfo;

template<class T> const TypeInfo* sge_typeof();
template<class T, class... ARGS> T* sge_creator(ARGS&&... args);

class FieldInfo {
public:
	const char* name = "";
	const TypeInfo* fieldInfo = nullptr;
	intptr_t offset = 0;

	template<class Obj, class Member>
	FieldInfo(const char* name_, Member Obj::*ptr)
		: name(name_)
	{
		fieldInfo = sge_typeof<Member>();
		offset = memberOffset(ptr);
	}

	void onFormat(fmt::format_context& ctx) const;
};

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
		return isKindOf(sge_typeof<DST>());
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
class TIBaseInitNoBase : public TypeInfo {
public:
	TIBaseInitNoBase(const char* name_, Style style_ = Style::Primitive) {
		name = name_;
		style = style_;
		dataSize = sizeof(T);
	}
};

template<class T, class BASE>
class TIBaseInit : public TIBaseInitNoBase<T> {
public:
	TIBaseInit(const char* name_, Style style_) : TIBaseInitNoBase<T>(name_, style_) {
		static_assert(std::is_base_of<BASE, T>::value, "invalid base class");
		base = sge_typeof<BASE>();
	}
};

template<class T> inline
const TypeInfo* sge_typeof<T>() {
	return T::s_getType();
}

template<class T, class... ARGS> inline
T* sge_creator(ARGS&&... args) {
	return new T(SGE_FORWARD(args)...);
}

#define sge_typeof_define(T) template<> const TypeInfo* sge_typeof<T>();

sge_typeof_define(u8)
sge_typeof_define(u16)
sge_typeof_define(u32)
sge_typeof_define(u64)

sge_typeof_define(i8)
sge_typeof_define(i16)
sge_typeof_define(i32)
sge_typeof_define(i64)

sge_typeof_define(f32)
sge_typeof_define(f64)
sge_typeof_define(f128)

SGE_FORMATTER(TypeInfo)
SGE_FORMATTER(FieldInfo)
} // namespace