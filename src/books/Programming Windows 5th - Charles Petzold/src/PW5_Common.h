#pragma once

#include <sge_core.h>
#include <sge_core/native_ui/NativeUI.h>

#if SGE_OS_WINDOWS
	#include <sge_core/native_ui/win32/Win32Util.h>
	#include <windowsx.h>
	#include <tchar.h>
#endif

#define SGE_DEFINEMARK__ITEM(SGE_ID, SGE_MARK) { SGE_ID, #SGE_ID, SGE_MARK }
#define SGE_DEFINEMARK(T) \
	struct T; \
	template<> const DefineMark* DefineMarkOf<T>(); \
	\
	struct T { \
		using This = T; \
		static  const DefineMark* s_getMark(); \
		virtual const DefineMark* getMark() const { return s_getMark(); } \
	}; \
// ------

namespace sge {

template<class T> inline void g_bzero(T& s) {
	memset(&s, 0, sizeof(s));
}

class PW5_NativeUIWindow;

class DefineMark : public NonCopyable {
public:

	struct Item {
		Item(int id_, const char* name_, const char* mark_)
			: id(id_)
			, name(name_)
			, mark(mark_) {}

		int	id;
		const char* name = "";
		const char* mark = "";
	};

	using NativeUIWindow = PW5_NativeUIWindow;
	using Getter = void (*)(int& v, const Item& item, const NativeUIWindow* obj);

	void invoke(int& v, const Item& item, const NativeUIWindow* obj) const {
		if (_getter) _getter(v, item, obj);
	}

	const Span<const Item> data() const { return _data; }

protected:
	template <size_t N>
	void set(const Item(&data)[N], Getter getter = nullptr) {
		_data = data;
		_getter = getter;
	}

private:
	Span<const Item> _data;
	Getter			 _getter = nullptr;
};

template<class T> inline const DefineMark* DefineMarkOf() { return T::s_getMark(); }
template<class T> inline const DefineMark* DefineMarkOf(const T&) { return DefineMarkOf<T>(); }


template<typename DST, class SRC, class ENABLE = std::enable_if_t<std::is_enum_v<SRC>>> inline
DST g_flags2Bits(SRC srcFlags, DST (*castor)(SRC) ) {
	DST res = 0;
	DST n = static_cast<DST>(srcFlags);
	while (n) {
		auto v = Math::lowbit(n);
		res |= castor(static_cast<SRC>(v));
		n -= v;
	}
	return res;
}

}