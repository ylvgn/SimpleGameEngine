#pragma once
#include <sge_core/log/Log.h>

namespace sge {

#define sge_typeof_define(T) template<> const TypeInfo* sge_typeof<T>();

class TypeInfo;

template<class T> const TypeInfo* sge_typeof();

class FieldInfo {
public:
	const char* name = "";
	const TypeInfo* fieldInfo = nullptr;
	intptr_t offset = 0;

	bool isArray = false;
	Vector<const TypeInfo*> fieldArrayInfo;

	template<class Obj, class Member>
	FieldInfo(const char* name_, Member Obj::*ptr)
		: name(name_)
	{
		if (std::is_array<Member>()) { // <--------------- todo
			isArray = true;
			size_t n = std::extent<Member>();
			for (size_t i = 0; i < n; i++) {
				using A = std::remove_extent<Member>::type; // how to handle Vector<UPtr<Component>> ?
				const TypeInfo* ti = sge_typeof<A>();
				fieldArrayInfo.emplace_back(ti);
			}
		}
		else {
			isArray = false;
			fieldInfo = sge_typeof<Member>();
			offset = memberOffset(ptr);
		}
	}

	void onFormat(fmt::format_context& ctx) const;
};

class TypeInfo {
public:
	const char* name = "";
	const TypeInfo* base = nullptr;
	size_t dataSize = 0;

	Span<FieldInfo> fieldArray;

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

	void onFormat(fmt::format_context& ctx) const;
};

class TypeManager;

template<class T>
class TIBaseInitNoBase : public TypeInfo {
public:
	TIBaseInitNoBase(const char* name_) {
		name = name_;
		dataSize = sizeof(T);
		TypeManager::instance()->registerType(this);
	}
};

template<class T, class BASE>
class TIBaseInit : public TIBaseInitNoBase<T> {
public:
	TIBaseInit(const char* name_) : TIBaseInitNoBase<T>(name_) {
		static_assert(std::is_base_of<BASE, T>::value, "invalid base class");
		base = sge_typeof<BASE>();
	}
};

template<class T> inline
const TypeInfo* sge_typeof<T>() {
#if 0 // temp
	if (std::is_array<T>::value) {
		return nullptr;
	}
#endif
	return T::s_getType();
}

// pod
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

sge_typeof_define(Vec2d)
sge_typeof_define(Vec3d)
sge_typeof_define(Vec4d)

sge_typeof_define(Vec2f)
sge_typeof_define(Vec3f)
sge_typeof_define(Vec4f)

sge_typeof_define(Quat4f)

SGE_FORMATTER(TypeInfo)
SGE_FORMATTER(FieldInfo)
} // namespace