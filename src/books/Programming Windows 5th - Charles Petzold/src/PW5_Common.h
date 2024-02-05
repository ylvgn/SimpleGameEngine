#pragma once

#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <sge_core/native_ui/win32/Win32Util.h>
	#include <windowsx.h>
	#include <tchar.h>
#endif

#define SGE_DEFINE_MARK__ITEM(SGE_ID, ...) { SGE_ID, TEXT(#SGE_ID), TEXT(SGE_STRINGIFY(__VA_ARGS__)) }
#define SGE_DEFINE_MARK_STRUCT(SGE_NAME) \
struct SGE_NAME; /* forward declare */ \
template<> const DefineMarkInfo* DefineMarkOf<SGE_NAME>(); \
\
struct SGE_NAME { \
	static const DefineMarkInfo* s_getMarkOf() { return DefineMarkOf<SGE_NAME>(); }; \
	virtual const DefineMarkInfo* getMarkOf() const { return s_getMarkOf(); } \
}; \
// ------

namespace sge {

template<class T> constexpr
void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

class DefineMark {
public:
	int iIndex;
	const wchar_t* szLabel = L"";
	const wchar_t* szDesc  = L"";
};

class DefineMarkInfo {
public:
	const DefineMark*	data = nullptr;
	size_t				dataSize = 0;

protected:
	template <size_t N>
	void setDefineMarks(const DefineMark (&data_)[N]) {
		data = data_;
		if (data_) dataSize = N;
		else dataSize = 0;
	}
};

template<class T> inline const DefineMarkInfo* DefineMarkOf() { return T::s_defineMarkOf(); }
template<class T> inline const DefineMarkInfo* DefineMarkOf(const T&) { return DefineMarkOf<T>(); }

}