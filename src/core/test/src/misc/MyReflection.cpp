#include "MyReflection.h"

namespace sge {

#define my_typeof_impl(T) \
template<> const MyTypeInfo* my_typeof<T>() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T) {} \
	}; \
	static TI ti; \
	return &ti; \
} \
// -------
my_typeof_impl(u8)
my_typeof_impl(u16)
my_typeof_impl(u32)
my_typeof_impl(u64)

my_typeof_impl(i8)
my_typeof_impl(i16)
my_typeof_impl(i32)
my_typeof_impl(i64)

my_typeof_impl(f32)
my_typeof_impl(f64)
my_typeof_impl(f128)

// String
my_typeof_impl(TempStringA)
my_typeof_impl(TempStringW)
my_typeof_impl(StringA)
my_typeof_impl(StringW)

#define my_typeof_stl_container_impl(T, E) \
template<> const MyTypeInfo* my_typeof<T>() { \
	class TI : public TIElementInit<T, E> { \
		public: \
			TI() : TIElementInit<T, E>(#T) {} \
	}; \
	static TI ti; \
	return &ti; \
} \
// ---------

// Vector
my_typeof_stl_container_impl(Vector<u8>, u8)
my_typeof_stl_container_impl(Vector<u16>, u16)
my_typeof_stl_container_impl(Vector<u32>, u32)
my_typeof_stl_container_impl(Vector<u64>, u64)

my_typeof_stl_container_impl(Vector<i8>, i8)
my_typeof_stl_container_impl(Vector<i16>, i16)
my_typeof_stl_container_impl(Vector<i32>, i32)
my_typeof_stl_container_impl(Vector<i64>, i64)

my_typeof_stl_container_impl(Vector<f32>, f32)
my_typeof_stl_container_impl(Vector<f64>, f64)
my_typeof_stl_container_impl(Vector<f128>, f128)

// Vector<String>
my_typeof_stl_container_impl(Vector<TempStringA>, TempStringA)
my_typeof_stl_container_impl(Vector<TempStringW>, TempStringW)
my_typeof_stl_container_impl(Vector<StringA>, StringA)
my_typeof_stl_container_impl(Vector<StringW>, StringW)

// StringMap
my_typeof_stl_container_impl(StringMap<u8>, u8)
my_typeof_stl_container_impl(StringMap<u16>, u16)
my_typeof_stl_container_impl(StringMap<u32>, u32)
my_typeof_stl_container_impl(StringMap<u64>, u64)

my_typeof_stl_container_impl(StringMap<i8>, i8)
my_typeof_stl_container_impl(StringMap<i16>, i16)
my_typeof_stl_container_impl(StringMap<i32>, i32)
my_typeof_stl_container_impl(StringMap<i64>, i64)

my_typeof_stl_container_impl(StringMap<f32>, f32)
my_typeof_stl_container_impl(StringMap<f64>, f64)
my_typeof_stl_container_impl(StringMap<f128>, f128)

#define my_typeof_impl_s_getType(T) \
const MyTypeInfo* T::s_getType() { \
	class TI : public TIBaseInitNoBase<T> { \
	public: \
		TI() : TIBaseInitNoBase<T>(#T) {} \
	}; \
	static TI ti; \
	return &ti; \
} \
// --------
my_typeof_impl_s_getType(MyObject)

void MyFieldInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, offset={}, type={}", name, offset, fieldInfo->name);
}

void MyTypeInfo::onFormat(fmt::format_context& ctx) const {
	fmt::format_to(ctx.out(), "name={}, dataSize={}", name, dataSize);
	if (base) {
		fmt::format_to(ctx.out(), ", base=[{}]", *base);
	}
	if (!fieldArray.empty()) fmt::format_to(ctx.out(), "{}", ", fields={");
	for (auto& f : fieldArray) {
		fmt::format_to(ctx.out(), " [{}],", f);
	}
	if (!fieldArray.empty()) fmt::format_to(ctx.out(), " {}", '}');
}

} // namespace