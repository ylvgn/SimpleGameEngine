#pragma once

#if SGE_OS_WINDOWS

#include "../PW5_NativeUI.h"

namespace sge {

#define SGE_DEFINEMARK__ITEM(SGE_ID, SGE_MARK) { SGE_ID, L#SGE_ID, SGE_MARK }
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

class PW5_MyDefineMarkWindow;

class DefineMark : public NonCopyable {
	using UIWindow = PW5_MyDefineMarkWindow;
public:
	struct Item {
		Item(int id_, const wchar_t* name_, const wchar_t* mark_)
			: id(id_)
			, name(name_)
			, mark(mark_) {}

		int			   id;
		const wchar_t* name = L"";
		const wchar_t* mark = L"";
	};

	using Getter = void (*)(void* v, const UIWindow* obj, const Item& item);

	void io(u8& v, const UIWindow* obj, const Item& item)  { toValue(v, obj, item); }
	void io(u16& v, const UIWindow* obj, const Item& item) { toValue(v, obj, item); }
	void io(u32& v, const UIWindow* obj, const Item& item) { toValue(v, obj, item); }
	void io(u64& v, const UIWindow* obj, const Item& item) { toValue(v, obj, item); }

	void io(i8& v, const UIWindow* obj, const Item& item)  { toValue(v, obj, item); }
	void io(i16& v, const UIWindow* obj, const Item& item) { toValue(v, obj, item); }
	void io(i32& v, const UIWindow* obj, const Item& item) { toValue(v, obj, item); }
	void io(i64& v, const UIWindow* obj, const Item& item) { toValue(v, obj, item); }

	const Span<const Item> data() const { return _data; }

protected:
	template <size_t N, typename V>
	void set(const Item(&data)[N],
			 void (*getter)(V& v, const UIWindow* obj, const Item& item) = nullptr)
	{
		_data   = data;
		_getter = reinterpret_cast<Getter>(getter);
	}

	template <typename V>
	void toValue(V& v, const UIWindow* obj, const Item& item) const {
		if (_getter)
			_getter(&v, obj, item);
	}

private:
	Span<const Item> _data;
	Getter			 _getter = nullptr;
};

template<class T> inline const DefineMark* DefineMarkOf() { return T::s_getMark(); }
template<class T> inline const DefineMark* DefineMarkOf(const T&) { return DefineMarkOf<T>(); }


class PW5_MyDefineMarkWindow : public PW5_NativeUIWindow {
	using Base = PW5_NativeUIWindow;
	using This = PW5_MyDefineMarkWindow;
protected:
	virtual void onCreate(CreateDesc& desc) override;
	virtual void onClientRectChanged(const Rect2f& rc) override;
	virtual void onUIScrollBarEvent(UIScrollBarEvent& ev) override;
	virtual void onDraw() override;

	int _cxChar, _cyChar, _cxCaps;

	const DefineMark* _dm = nullptr;
};

SGE_DEFINEMARK(MySysmetricsDM)

}

#endif