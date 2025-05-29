#pragma once

#include "sge_core-config.h"

#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS 1 // Enable getenv
#endif

#include "../detect_platform/sge_detect_platform.h"

//---- os
#if SGE_OS_WINDOWS
	#define NOMINMAX 1
	#include <WinSock2.h> // WinSock2.h must include before windows.h to avoid winsock1 define
	#include <ws2tcpip.h> // struct sockaddr_in6
	#pragma comment(lib, "Ws2_32.lib")
	#include <Windows.h>
	#include <intsafe.h>
	#include <xmmintrin.h>

	#ifndef SGE_TRY_USE_OPENGL
		#error
	#elif SGE_TRY_USE_OPENGL
		#define GLEW_STATIC 1
		// include glew before gl.h
		#include <sge_core/glew/glew.h>
		#include <sge_core/glew/wglew.h>

		#include <GL/gl.h>
		#include <GL/glu.h>

		#if SGE_COMPILER_VC
			#pragma comment(lib, "Opengl32.lib") // gl.h
			#pragma comment(lib, "GLu32.lib")	 // glu.h
		#endif
		// Note: glaux has been deprecated for a long time
		// #include <Gl/glaux.h>
		// #pragma comment(lib, "GLaux.lib")
	#endif

#else
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <netinet/in.h> // struct sockaddr_in
#endif

//---- cpp
#include <cassert>
#include <exception>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <atomic>
#include <functional>

//---- externals
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/list.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <EASTL/string_view.h>
#include <EASTL/span.h>
#include <EASTL/optional.h>
#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector_map.h>
#include <EASTL/string_map.h>
#include <EASTL/set.h>
#include <EASTL/bitset.h>
#include <EASTL/any.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/weak_ptr.h>

#include <nlohmann/json.hpp>

//----
#include "sge_macro.h"

using Json = nlohmann::json;

//==== EASTL ====

#if !EASTL_DLL // If building a regular library and not building EASTL as a DLL...
	// It is expected that the application define the following
	// versions of operator new for the application. Either that or the
	// user needs to override the implementation of the allocator class.
inline void* operator new[](size_t size, const char* pName, int flags, unsigned debugFlags, const char* file, int line) { return malloc(size); }
inline void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) {
#if SGE_OS_WINDOWS
	return _aligned_malloc(size, alignment);
#else
	return std::aligned_alloc(alignment, size);
#endif
}
#endif

//==== EASTL ====

namespace sge {

template<class T> inline constexpr typename std::underlying_type<T>::type  enumInt(T  value) { return static_cast<typename std::underlying_type<T>::type>(value); }
template<class T> inline constexpr typename std::underlying_type<T>::type& enumIntRef(T& value) { return *reinterpret_cast<typename std::underlying_type<T>::type*>(&value); }
template<class T> inline constexpr typename std::underlying_type<T>::type const& enumIntRef(const T& value) { return *reinterpret_cast<const typename std::underlying_type<T>::type*>(&value); }

template<class T> inline bool constexpr enumHas(const T& a, const T& b) { return static_cast<T>(enumInt(a) & enumInt(b)) != static_cast<T>(0); }

template<class T> SGE_INLINE T* constCast(const T* v) { return const_cast<T*>(v); }
template<class T> SGE_INLINE T& constCast(const T& v) { return const_cast<T&>(v); }

template<class T> SGE_INLINE void swap(T& a, T& b) { T tmp = SGE_MOVE(a); a = SGE_MOVE(b); b = SGE_MOVE(tmp); }

using u8   = uint8_t;
using u16  = uint16_t;
using u32  = uint32_t;
using u64  = uint64_t;
using u128 = uint128_t;

using i8   = int8_t;
using i16  = int16_t;
using i32  = int32_t;
using i64  = int64_t;
using i128 = int128_t;

//using f16 = half;
using f32	= float;
using f64	= double;
using f128	= long double;

using StrLiteral = const char*;

template< class Obj, class Member > constexpr
intptr_t memberOffset(Member Obj::*ptrToMember) {
	Obj* c = nullptr;
	Member* m = &(c->*ptrToMember);
	return reinterpret_cast<intptr_t>(m);
}

template<class T> constexpr
size_t charStrlen(const T* sz) {
	const auto* p = sz;
	while (*p) ++p;
	return static_cast<size_t>(p - sz);
}

template<class T> using UPtr = eastl::unique_ptr<T>;
template <class T, class... Args> inline UPtr<T> UPtr_make(Args&&... args) {
	return eastl::make_unique<T>(SGE_FORWARD(args)...);
}

template<class T> using Span = eastl::span<T>; // mutable
using ByteSpan = Span<const u8>;

template<class DST, class SRC> inline 
Span<DST> spanCast(Span<SRC> src) {
	size_t sizeInBytes = src.size() * sizeof(SRC);
	return Span<DST>(reinterpret_cast<DST*>(src.data()), sizeInBytes / sizeof(DST));
}

template<class T, size_t N, bool bEnableOverflow = true>
struct Vector_Base {
	using Type = typename eastl::fixed_vector<T, N, bEnableOverflow>;
};

template<class T>
struct Vector_Base<T, 0, true> {
	using Type = typename eastl::vector<T>;
};

template<class T, size_t N = 0, bool bEnableOverflow = true>
class Vector : public Vector_Base<T, N, bEnableOverflow>::Type {
	using Base = typename Vector_Base<T, N, bEnableOverflow>::Type;
	using This = Vector;
public:
	using Base::begin;
	using Base::end;

	Vector() = default;
	Vector(std::initializer_list<T> list) : Base(list) {}

	void appendRange(const Span<const T>& rhs) { Base::insert(end(), rhs.begin(), rhs.end()); }

	Span<      T> span()			{ return Span<      T>(begin(), end()); }
	Span<const T> span() const		{ return Span<const T>(begin(), end()); }

	operator Span<      T>()		{ return span(); }
	operator Span<const T>() const	{ return span(); }

	Span<      T> subspan(size_t offset, size_t count)			{ return Span<      T>(begin() + offset, count); }
	Span<const T> subspan(size_t offset, size_t count) const	{ return Span<const T>(begin() + offset, count); }

	Span<      T> subspan(size_t offset)		{ return subspan(offset, size() - offset); }
	Span<const T> subspan(size_t offset) const	{ return subspan(offset, size() - offset); }

	bool inBound(int i) const { return i >= 0 && i < Base::size(); }

	void remove(const T& value) { eastl::remove(begin(), end(), value); }

	void resizeToLocalBufSize() { Base::resize(N); }

	void move(This& rhs) { Base::clear(); *this = eastl::move(rhs); }
};

template<class T> using List_Base = typename eastl::list<T>;
template<class T>
class List : public List_Base<T> {
	using Base		= typename List_Base<T>;
	using size_type = typename Base::size_type;
public:
	using Base::begin;
	using Base::end;

			 List() = default;
	explicit List(size_type n)						: Base(n)	 {}
			 List(std::initializer_list<T> list)	: Base(list) {}

	Span<      T> span()			{ return Span<      T>(begin(), end()); }
	Span<const T> span() const		{ return Span<const T>(begin(), end()); }

	operator Span<      T>()		{ return span(); }
	operator Span<const T>() const	{ return span(); }

	void appendRange(const Span<const T>& r) { Base::insert_range(r.begin(), r.end()); }

	bool inBound(int i) const { return i >= 0 && i < Base::size(); }
};

template<class KEY, class VALUE>	using Map		= eastl::map<KEY, VALUE>;
template<class KEY, class VALUE>	using VectorMap	= eastl::vector_map<KEY, VALUE>;
template<class VALUE>				using StringMap	= eastl::string_map<VALUE>;
template<class KEY>					using Set		= eastl::set<KEY>;
template<size_t N>					using Bitset	= eastl::bitset<N>;
template<class T>					using Opt		= eastl::optional<T>;

using Any = eastl::any;

template <class T, class... Args> inline
Any Any_make(Args&&... args) {
	return eastl::make_any<T>(SGE_FORWARD(args)...);
}

template<class T, size_t N, bool bEnableOverflow = true> class StringT; // forward declare

template<class T> using StrViewT_Base = eastl::basic_string_view<T>;
template<class T>
class StrViewT : public StrViewT_Base<T> { // immutable string view
	using Base			= typename StrViewT_Base<T>;
	using size_type		= typename Base::size_type;
	using const_pointer = typename Base::const_pointer;

	using Base::mnCount;
	using Base::mpBegin;
public:
						EA_CONSTEXPR StrViewT() = default;
						EA_CONSTEXPR StrViewT(const StrViewT& other)				EA_NOEXCEPT : Base(other) {}
						EA_CONSTEXPR StrViewT(const T* s)							EA_NOEXCEPT : Base(s) {}
						EA_CONSTEXPR explicit StrViewT(const T* s, size_type count)	EA_NOEXCEPT : Base(s, count) {}
						EA_CONSTEXPR explicit StrViewT(T& ch)						EA_NOEXCEPT : Base(&ch, 1) {}
						EA_CONSTEXPR StrViewT(const Base& s)						EA_NOEXCEPT : Base(s.data(), s.size()) {}
	template<size_t N>	EA_CONSTEXPR StrViewT(const StringT<T, N>& s)				EA_NOEXCEPT : Base(s.data(), s.size()) {}

	explicit operator bool() const { return !empty(); }

	const T&	operator[]	(int i)		const	{ return at(i); }
	const T&	at			(int i)		const	{ _checkBound(i); return mpBegin[i]; }
	const T&	unsafe_at	(int i)		const	{ return mpBegin[i]; }
	 	  T*	tryAt(int i)					{ return inBound(i) ? &mpBegin[i] : nullptr; }
	const T*	tryAt(int i)			const	{ return inBound(i) ? &mpBegin[i] : nullptr; }
	const T&	back		(int i = 0)	const	{ return at(mnCount - i - 1); }
	const T&	unsafe_back	(int i = 0)	const	{ unsafe_at(mnCount - i - 1); }

			bool inBound(int i)			const	{ return i >= 0 && i < mnCount; }

	StrViewT<T> extractFromPrefix(StrViewT prefix) const {
		return starts_with(prefix) ? sliceFrom(prefix.size()) : StrViewT();
	}

	StrViewT<T> sliceFrom(int offset)			const { return slice(offset, mnCount - offset); }
	StrViewT<T> sliceBack(int offset)			const { return slice(mnCount - offset, offset); }
	StrViewT<T> sliceFromAndBack(int offset)	const { return slice(offset, mnCount - offset - offset); }

	StrViewT<T> slice(int offset, int size) const {
		if (offset < 0 || size < 0 || offset + size > mnCount)
			throw std::out_of_range("StrViewT::slice");
		return StrViewT(begin() + offset, size);
	}

private:
	void _checkBound(int i) const {
		if (!inBound(i))
			throw std::out_of_range("StrViewT::_checkBound");
	}
};

using StrViewA  = StrViewT<char>;
using StrViewW  = StrViewT<wchar_t>;

using StrView8  = StrViewT<char>; // char8_t: require c++20
using StrView16 = StrViewT<char16_t>;
using StrView32 = StrViewT<char32_t>;

template<class T, size_t N, bool bEnableOverflow = true>
struct StringT_Base {
	using Type = typename eastl::fixed_string<T, N, bEnableOverflow>;
};

template<class T>
struct StringT_Base<T, 0, true> {
	using Type = typename eastl::basic_string<T>;
};

template<class T, size_t N, bool bEnableOverflow = true>
class StringT : public StringT_Base<T, N, bEnableOverflow>::Type {
	using Base = typename StringT_Base<T, N, bEnableOverflow>::Type;
public:
	using view_type = typename eastl::basic_string_view<T>;
	using StrViewT	= typename StrViewT<T>;

	using Base::npos;
	using Base::append;

						StringT() = default;
						StringT(const T* begin, const T* end)	EA_NOEXCEPT : Base(begin, end) {}
						StringT(const T* p, size_t n)			EA_NOEXCEPT : Base(p, n) {}
						StringT(StrViewT view)					EA_NOEXCEPT : Base(view.data(), view.size()) {}
						StringT(StringT&& str)					EA_NOEXCEPT : Base(std::move(str)) {}
						StringT(const T* sz)					EA_NOEXCEPT : Base(sz) {}
						StringT(const StringT& s)				EA_NOEXCEPT : Base(s.data(), s.size()) {}
						StringT(const Base& s)					EA_NOEXCEPT : Base(s.data(), s.size()) {}
	template<size_t M>	StringT(const StringT<T, M>& s)			EA_NOEXCEPT : Base(s.data(), s.size()) {}

	explicit operator bool() const { return !Base::empty(); }

						void operator= (const StringT& s)		{ Base::assign(s.data(), s.size()); }
	template<size_t N>	void operator= (const StringT<T, N>& r)	{ Base::assign(s.data(), s.size()); }
	template<class R>	void operator= (R&& r)					{ Base::operator=(SGE_FORWARD(r)); }

						void operator+=(StrViewT v)				{ Base::append(v.begin(), v.end()); }
	template<size_t N>	void operator+=(const StringT<T, N>& v) { Base::append(v.begin(), v.end()); }
	template<class R>	void operator+=(const R& r)				{ Base::operator+=(r); }

						void append(const StrViewT& s)			{ Base::append(s.data(), s.size()); }
						void append(T& ch)						{ Base::append(StrViewT(ch)); }
						void append(const StringT& s)			{ Base::append(s.data(), s.size()); }
	template<size_t M>	void append(const StringT<T, M>& s)		{ Base::append(s.data(), s.size()); }

	template<class... ARGS>	void append(ARGS&&... args) {
		StrViewT views[]{ SGE_FORWARD(args)... };
		size_t n = sizeof...(args);
		if (!n) return;
		for (size_t i = 0; i < n; ++i) {
			const auto& sv = views[i];
			if (sv) append(sv);
		}
	}

	template<class... ARGS> void set(ARGS&&... args) { Base::clear(); append(SGE_FORWARD(args)...); }

	StrViewT	view() const { return StrViewT(data(), size()); }

	void replaceChars(T from, T to) {
		auto* s = begin();
		auto* e = end();
		for ( ; s < e; ++ s) {
			if (*s == from) *s = to;
		}
	}

	void resizeToLocalBufSize() { Base::resize(N); }
};

template<size_t N, bool bEnableOverflow = true> using StringA_ = StringT<char,    N, bEnableOverflow>;
template<size_t N, bool bEnableOverflow = true> using StringW_ = StringT<wchar_t, N, bEnableOverflow>;

template<size_t N, bool bEnableOverflow = true> using String8_  = StringT<char,		N, bEnableOverflow>; // char8_t: require c++20
template<size_t N, bool bEnableOverflow = true> using String16_ = StringT<char16_t, N, bEnableOverflow>;
template<size_t N, bool bEnableOverflow = true> using String32_ = StringT<char32_t, N, bEnableOverflow>;

using TempStringA = StringA_<220>;
using TempStringW = StringW_<220>;

using StringA = StringA_<0>;
using StringW = StringW_<0>;

using StrView = StrViewA;
using String  = StringA;

template<class T, size_t N> inline bool operator==(const StringT<T, N>& lhs, StrViewT<T> rhs) { return lhs.compare(rhs.data()) == 0; }
template<class T, size_t N> inline bool operator!=(const StringT<T, N>& lhs, StrViewT<T> rhs) { return lhs.compare(rhs.data()) != 0; }

template<class T, size_t N, size_t M> inline bool operator==(const StringT<T, N>& lhs, const StringT<T, M>& rhs) { return lhs.compare(rhs.data()) == 0; }
template<class T, size_t N, size_t M> inline bool operator!=(const StringT<T, N>& lhs, const StringT<T, M>& rhs) { return lhs.compare(rhs.data()) != 0; }

template<class T, size_t N> inline bool operator==(StrViewT<T> lhs, const StringT<T, N>& rhs) { return rhs.compare(lhs.data()) == 0; }
template<class T, size_t N> inline bool operator!=(StrViewT<T> lhs, const StringT<T, N>& rhs) { return rhs.compare(lhs.data()) != 0; }

template<class T> inline bool operator==(StrViewT<T> lhs, const T* rhs) { return lhs.compare(rhs) == 0; }
template<class T> inline bool operator!=(StrViewT<T> lhs, const T* rhs) { return lhs.compare(rhs) != 0; }

template<class T, size_t N> inline
StringT<T, N> operator+(const StringT<T, N>& a, StrViewT<T> b) {
	StringT<T, N> o;
	o.reserve(a.size() + b.size());
	o.append(a.begin(), a.end());
	o.append(b.begin(), b.end());
	return o;
}

template<class T, size_t N, size_t M> inline
StringT<T, N> operator+(const StringT<T, N>& a, const StringT<T, M>& b) {
	return a + b.view();
}

template<class T, size_t N> inline
StringT<T, N> operator+(const StringT<T, N>& a, const T* b) {
	StrViewT<T> view(b, charStrlen(b));
	return a + view;
}

SGE_NODISCARD inline StrView StrView_make(ByteSpan s) {
	return StrView(reinterpret_cast<const char*>(s.data()), s.size());
}

SGE_NODISCARD inline ByteSpan ByteSpan_make(StrView v) {
	return ByteSpan(reinterpret_cast<const u8*>(v.data()), v.size());
}

template<class SRC> inline
ByteSpan ByteSpan_make(Span<SRC> v) {
	using DST = typename ByteSpan::element_type;
	return spanCast<DST>(v);
}

template<size_t N> using String_ = StringA_<N>;
using TempString = TempStringA;

template<size_t N> struct CharBySize;
template<> struct CharBySize<1> { using Type = char; }; // char8_t: require c++20
template<> struct CharBySize<2> { using Type = char16_t; };
template<> struct CharBySize<4> { using Type = char32_t; };

struct WCharUtil {
	using Char = typename CharBySize<sizeof(wchar_t)>::Type;
	Char    toChar (wchar_t c) { return static_cast<Char>(c); }
	wchar_t toWChar(Char    c) { return static_cast<wchar_t>(c); }
};

inline StrView   StrView_c_str(const char*		s)	{ return s ? StrView  (s, strlen(s))	 : StrView (); } // char8_t: require c++20
inline StrView16 StrView_c_str(const char16_t*	s)	{ return s ? StrView16(s, charStrlen(s)) : StrView16(); }
inline StrView32 StrView_c_str(const char32_t*	s)	{ return s ? StrView32(s, charStrlen(s)) : StrView32(); }
inline StrViewW  StrView_c_str(const wchar_t*	s)	{ return s ? StrViewW (s, wcslen(s))	 : StrViewW(); }

//! Source Location
class SrcLoc {
public:
	SGE_INLINE SrcLoc(const char* file_, int line_, const char* func_) noexcept
		: file(file_)
		, func(func_)
		, line(line_)
	{}

	const char* file = "";
	const char* func = "";
	int line = 0;
};

} // namespace sge

#include "TypeTraits.h"

namespace sge {

class NonCopyable {
public:
	NonCopyable() = default;
private:
	NonCopyable		(const NonCopyable&) = delete;
	void operator=	(const NonCopyable&) = delete;
};

class WeakRefBlock : public NonCopyable {
public:
	void* _obj = nullptr;
	std::atomic_int	_weakCount = 0;
};

class RefCountBase : public NonCopyable {
public:
	WeakRefBlock*	_weakRefBlock = nullptr;
	std::atomic_int	_refCount = 0;
};

template<class T> inline void sge_delete(T* p) noexcept { delete p; }

template<class T>
class ScopedValue : public NonCopyable {
public:
	explicit ScopedValue(T& p)						noexcept : _p(&p) { _oldValue = p; }
	explicit ScopedValue(T& p, const T& newValue)	noexcept : ScopedValue(p) { p = newValue; }

	ScopedValue(ScopedValue && r) noexcept {
		_p = r._p;
		_oldValue = r._oldValue;
		r._p = nullptr;
	}

	~ScopedValue() noexcept { discard(); }

	void discard() noexcept {
		if (_p) {
			*_p = _oldValue;
			_p = nullptr;
		}
	}

	explicit operator bool() const { return _p != nullptr; }

	T* ptr()		{ return _p; }
	T& value()		{ return *_p; }

	T* operator->() { return _p; }

private:
	T* _p = nullptr;
	T _oldValue;
};

template<class T> SGE_NODISCARD inline ScopedValue<T> ScopedValue_make(T& p)					{ return ScopedValue<T>(p); }
template<class T> SGE_NODISCARD inline ScopedValue<T> ScopedValue_make(T& p, const T& newValue) { return ScopedValue<T>(p, newValue); }


template<class First, class Second>
struct Pair {
	Pair() = delete;

	constexpr Pair(const First& first_, const Second& second_) noexcept : first(first_), second(second_) {}
	constexpr Pair(First&& first_, Second&& second_)		   noexcept : first(SGE_MOVE(first_)), second(SGE_MOVE(second_)) {}

	First	first;
	Second	second;
};

template<class First, class Second> inline
Pair<First, Second> Pair_make(const First& first, const Second& second) {
	return Pair<First, Second>(first, second);
}

template<class First, class Second = typename First> inline
Pair<First, Second> Pair_make() {
	return Pair<First, Second>(First(), Second());
}

} // namespace sge
