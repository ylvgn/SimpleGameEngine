#pragma once

#include "SPtr.h"

namespace sge {

template<class T>
class WeakPtr : public NonCopyable {
public:
	WeakPtr() = default;

	WeakPtr(T* p) noexcept						{ reset(p);}
	WeakPtr(WeakPtr && r) noexcept				{ _p = r.detach(); }
	WeakPtr(const WeakPtr& r) noexcept			{ reset(r._p); }

	void operator=(T* p) noexcept				{ reset(p); }
	void operator=(WeakPtr && r) noexcept		{ reset(nullptr); _p = r.detach(); }
	void operator=(const WeakPtr& r) noexcept	{ reset(r._p); }

	~WeakPtr() noexcept { reset(nullptr); }

			SPtr<T> toSPtr()		noexcept { return _p ? SPtr<T>(_p->_refCount <= 0 ? nullptr : _p) : nullptr; }
	const	SPtr<T> toSPtr() const	noexcept { return _p ? SPtr<T>(_p->_refCount <= 0 ? nullptr : _p) : nullptr; }

	void reset(T* p) noexcept {
		static_assert(std::is_base_of<RefCountBase, T>::value, "");
		if (p == _p) return;
		if (_p) {
			auto c = --_p->_weakCount;
			if (c <= 0 && _p->_refCount <= 0) free(_p);
		}
		_p = p;
		if (p) {
			++p->_weakCount;
		}
	}
private:
	T* detach() noexcept { T* o = _p; _p = nullptr; return o; }

	T* _p = nullptr;
};

}