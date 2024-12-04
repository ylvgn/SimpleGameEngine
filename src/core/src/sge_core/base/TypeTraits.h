#pragma once

namespace sge { namespace TypeTraits {

template<class A, class B> struct isSame {
	static constexpr bool value = std::is_same<A, B>::value;
};

template<class B, class T> struct isBaseOf {
	static constexpr bool value = std::is_base_of<B, T>::value;
};

template<class T> struct isUnsigned	{
	static constexpr bool value = std::is_unsigned<T>::value;
};

template<class T> struct isSigned {
	static constexpr bool value = std::is_signed<T>::value;
};

template<class T> struct removeRef					{ using Type = T; };
template<class T> struct removeRef<T&&>				{ using Type = T; };
template<class T> struct removeRef<T&>				{ using Type = T; };

template<class T> struct removeConst				{ using Type = T; };
template<class T> struct removeConst<const T>		{ using Type = T; };

template<class T> struct removeConstRef				{ using Type = T; };
template<class T> struct removeConstRef<T&>			{ using Type = T; };
template<class T> struct removeConstRef<T&&>		{ using Type = T; };
template<class T> struct removeConstRef<const T>	{ using Type = T; };
template<class T> struct removeConstRef<const T&>	{ using Type = T; };

//----
template<size_t N> struct typeBySize;

template<>
struct typeBySize<1> {
	using Int  = i8;
	using UInt = u8;
	using Char = char;
};

template<>
struct typeBySize<2> {
	using Int = i16;
	using UInt = u16;
	using Char = char16_t;
};

template<>
struct typeBySize<4> {
	using Int   = i32;
	using UInt  = u32;
	using Float = f32;
	using Char  = char32_t;
};

template<>
struct typeBySize<8> {
	using Int   = i64;
	using UInt  = u64;
	using Float = f64;
};

template<>
struct typeBySize<16> {
	using Float = f128;
};

}} // namespace sge/TypeTraits