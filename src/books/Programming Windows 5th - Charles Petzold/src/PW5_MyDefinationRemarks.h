#pragma once

#if SGE_OS_WINDOWS

namespace sge {

#define PW5_MYDEFINATIONREMARKS__ITEM(ID, REMARKS) { ID, #ID, REMARKS }
#define PW5_MYDEFINATIONREMARKS(T) \
	struct T; \
	template<> const PW5_MyDefinationRemarks* DefineMarkOf<T>(); \
	\
	struct T { \
		using This = T; \
		static const PW5_MyDefinationRemarks* s_getRemarks(); \
		virtual const PW5_MyDefinationRemarks* getRemarks() const { return s_getRemarks(); } \
	}; \
// ------

class PW5_NativeUIWindow;

struct PW5_MyDefinationRemarks_Item : public NonCopyable {
	PW5_MyDefinationRemarks_Item() = delete;
	PW5_MyDefinationRemarks_Item(int id_, StrView name_, StrView remarks_)
		: id(id_)
		, name(name_)
		, remarks(remarks_) {}

	int	id;
	String name;
	String remarks;
};

class PW5_MyDefinationRemarks : public NonCopyable {
public:
	using Item				= PW5_MyDefinationRemarks_Item;
	using NativeUIWindow	= PW5_NativeUIWindow;
	using Getter			= void (*)(int& v, const Item& item, const NativeUIWindow* obj);

	void invoke(int& v, const Item& item, const NativeUIWindow* obj) const {
		if (_getter)
			_getter(v, item, obj);
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

template<class T> inline const PW5_MyDefinationRemarks* DefineMarkOf()			{ return T::s_getMark(); }
template<class T> inline const PW5_MyDefinationRemarks* DefineMarkOf(const T&)	{ return DefineMarkOf<T>(); }

}

#endif