#pragma once

namespace sge {

template<class T>
class SPtr : public NonCopyable {
public:
	SPtr() noexcept = default;

	SPtr(T* p)						noexcept { reset(p); }
	SPtr(SPtr && r)					noexcept { _p = r.detach(); }
	SPtr(const SPtr& r)				noexcept { reset(r._p); }

	void operator=(T* p)			noexcept { reset(p); }
	void operator=(SPtr && r)		noexcept { reset(nullptr); _p = r.detach(); }
	void operator=(const SPtr& r)	noexcept { reset(r._p); }

	~SPtr()							noexcept { reset(nullptr); }

		  T* operator->()			noexcept { return _p; }
	const T* operator->()	const	noexcept { return _p; }

	operator       T*()				noexcept { return _p; }
	operator const T*()		const	noexcept { return _p; }

			T* ptr()				noexcept { return _p; }
	const	T* ptr()		const	noexcept { return _p; }

	void reset(T* p) noexcept {
		SGE_STATIC_ASSERT(TypeTraits::isBaseOf<RefCountBase, T>::value);
		if (p == _p) return;
		if (_p) {
			auto c = --_p->_refCount;
			if (c <= 0) {
				if (_p->_weakRefBlock) {
					_p->_weakRefBlock->_obj = nullptr;
					_p->_weakRefBlock = nullptr;
				}
				sge_delete(_p);
			}
		}
		_p = p;
		if (_p) {
			++_p->_refCount;
		}
	}

	T* detach() noexcept { T* o = _p; _p = nullptr; return o; }
private:
	T* _p = nullptr;
};

}