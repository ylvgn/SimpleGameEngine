#pragma once

#include "SPtr.h"

namespace sge {

template<class T>
class WPtr : public NonCopyable {
public:
	WPtr() = default;
	WPtr(T* p)			noexcept { reset(p);}
	WPtr(WPtr && r)		noexcept { _p = r._detach(); _block = SGE_MOVE(r._block); }
	WPtr(const WPtr& r)	noexcept { reset(r._p); }

	~WPtr()				noexcept { reset(nullptr); }

	void operator=(T* p)				noexcept { reset(p); }
	void operator=(WPtr && r)			noexcept { _p = r._detach(); _block = SGE_MOVE(r._block); }
	void operator=(const WPtr& r)		noexcept { reset(r._p); }
	void operator=(const SPtr<T>& r)	noexcept { reset(constCast(r.ptr())); }

			SPtr<T> toSPtr()			noexcept { return _block ? SPtr<T>(static_cast<T*>(_block->_obj)) : nullptr; }
	const	SPtr<T> toSPtr()	const	noexcept { return _block ? SPtr<T>(static_cast<T*>(_block->_obj)) : nullptr; }

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
	T* _detach() noexcept { T* o = _p; _p = nullptr; return o; }

	T*				_p		= nullptr;
	WeakRefBlock*	_block	= nullptr;
}; // WPtr

} // namespace sge