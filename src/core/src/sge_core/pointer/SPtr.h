#pragma once

namespace sge {

template<class T>
class SPtr : public NonCopyable {
public:
	SPtr()				noexcept = default;
	SPtr(T* p)			noexcept { reset(p); }
	SPtr(SPtr&& r)		noexcept { _p = r.detach(); }
	SPtr(const SPtr& r)	noexcept { reset(r._p); }

	~SPtr()				noexcept { reset(nullptr); }

	void operator=(T* p)			noexcept { reset(p); }
	void operator=(SPtr&& r)		noexcept { reset(nullptr); _p = r.detach(); }
	void operator=(const SPtr& r)	noexcept { reset(r._p); }

		  T* operator->()			noexcept { return _p; }
	const T* operator->()	const	noexcept { return _p; }

	bool operator!() const	noexcept { return _p == nullptr; }

	operator T* () & { return _p; }
	operator T* () && = delete;

	explicit operator bool() const { return _p != nullptr; }

			T* ptr()		noexcept { return _p; }
	const	T* ptr() const	noexcept { return _p; }

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
}; // SPtr


template<class T> SGE_INLINE bool operator==(const SPtr<T>& l, const SPtr<T>& r)		noexcept { return l.ptr() == r.ptr(); }
template<class T> SGE_INLINE bool operator!=(const SPtr<T>& l, const SPtr<T>& r)		noexcept { return l.ptr() != r.ptr(); }

template<class T> SGE_INLINE bool operator==(const SPtr<T>& l, const T*& r)				noexcept { return l.ptr() == r; }
template<class T> SGE_INLINE bool operator!=(const SPtr<T>& l, const T*& r)				noexcept { return l.ptr() != r; }
template<class T> SGE_INLINE bool operator==(const T*& l, const SPtr<T>& r)				noexcept { return l == r.ptr(); }
template<class T> SGE_INLINE bool operator!=(const T*& l, const SPtr<T>& r)				noexcept { return l != r.ptr(); }

template<class T> SGE_INLINE bool operator==(const SPtr<T>& l, const std::nullptr_t&)	noexcept { return l.ptr() == nullptr; }
template<class T> SGE_INLINE bool operator==(const std::nullptr_t&, const SPtr<T>& r)	noexcept { return r.ptr() == nullptr; }
template<class T> SGE_INLINE bool operator!=(const SPtr<T>& l, const std::nullptr_t&)	noexcept { return l.ptr() != nullptr; }
template<class T> SGE_INLINE bool operator!=(const std::nullptr_t&, const SPtr<T>& r)	noexcept { return r.ptr() != nullptr; }

} // namespace sge