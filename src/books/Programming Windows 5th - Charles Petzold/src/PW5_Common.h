#pragma once

#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <sge_core/native_ui/win32/Win32Util.h>
	#include <windowsx.h>
	#include <tchar.h>
#endif

#define SGE_DEFINE_MARK__ITEM(SGE_ID, SGE_MARK) { SGE_ID, L#SGE_ID, SGE_MARK }

#define SGE_DEFINE_MARK_STRUCT(SGE_CLASS) \
struct SGE_CLASS; /* forward declare */ \
template<> const DefineMark* DefineMarkOf<SGE_CLASS>(); \
\
struct SGE_CLASS { \
	static  const DefineMark* s_getMarkOf()     { return DefineMarkOf<SGE_CLASS>(); }; \
	virtual const DefineMark* getMarkOf() const { return s_getMarkOf(); } \
}; \
// ------

namespace sge {

template<class T> constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

class DefineMark {
public:
	struct Item {
		int			   id;
		const wchar_t* name = L"";
		const wchar_t* mark = L"";
	};

	Span<const Item> data() const { return _data; }
	size_t size()			const { return _data.size(); }

protected:
	template <size_t N>
	void set(const Item(&data)[N]) { _data = data; }

private:
	Span<const Item> _data;
};

template<class T> inline const DefineMark* DefineMarkOf() { return T::s_defineMarkOf(); }
template<class T> inline const DefineMark* DefineMarkOf(const T&) { return DefineMarkOf<T>(); }

}