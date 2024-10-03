#pragma once

#include "SPtr.h"

namespace sge {

template<class T>
class WPtr : public NonCopyable {
public:
	WPtr() noexcept = default;

	WPtr(T* p)						noexcept { reset(p);}
	WPtr(WPtr && r)					noexcept { _p = r.detach(); }
	WPtr(const WPtr& r)				noexcept { reset(r._p); }

	void operator=(T* p)			noexcept { reset(p); }
	void operator=(WPtr && r)		noexcept { reset(nullptr); _p = r.detach(); }
	void operator=(const WPtr& r)	noexcept { reset(r._p); }

	~WPtr()							noexcept { reset(nullptr); }

			SPtr<T> toSPtr()		noexcept { return _block ? SPtr<T>(static_cast<T*>(_block->_obj)) : nullptr; }
	const	SPtr<T> toSPtr() const	noexcept { return _block ? SPtr<T>(static_cast<T*>(_block->_obj)) : nullptr; }

	void reset(T* p) noexcept {
		SGE_STATIC_ASSERT(TypeTraits::isBaseOf<RefCountBase, T>::value);
		if (p == _p) return;
		if (_p) {
			auto c = --_block->_weakCount;
			if (c <= 0) sge_delete(_block);
		}
		_block = nullptr;
		_p = p;
		if (_p) {
			_block = _p->_weakRefBlock;
			if (!_block) {
				_block = new WeakRefBlock();
				_p->_weakRefBlock = _block;
				_block->_obj = _p;
			}
			++_block->_weakCount;
		}
	}
private:
	T* detach() noexcept { T* o = _p; _p = nullptr; return o; }

	T*				_p		= nullptr;
	WeakRefBlock*	_block	= nullptr;
};

}