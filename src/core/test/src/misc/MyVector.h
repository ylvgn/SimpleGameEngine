#pragma once
// vedio: https://www.youtube.com/watch?v=tYQjfH_G-Vc&list=PLeGk08zVu454bRo93zNQa2MagJJ6Lv2mQ&index=1
// source from: https://github.com/SimpleTalkCpp/SimpleTalkCpp_Tutorial/blob/main/Advance%20C%2B%2B/001%20MyVector/MyVector.h
#include <cstdlib>
#include <new>

namespace sge {

template<class T>
class MyVector : NonCopyable {
public:
			T& unsafeAt(int i)				{ return m_data[i];						}
	const	T& unsafeAt(int i) const		{ return m_data[i];						}

			T& at(int i)					{ inBoundCheck(i); return m_data[i];	}
	const	T& at(int i) const				{ inBoundCheck(i); return m_data[i];	}

			T& operator[](int i)			{ return at(i);							}
	const	T& operator[](int i) const		{ return at(i);							}

			T& back()						{ return at(m_size - 1);				}
	const	T& back() const					{ return at(m_size - 1);				}

			T& back(int i)					{ return at(m_size - i - 1);			}
	const	T& back(int i) const			{ return at(m_size - i - 1);			}


	int size() const		{ return m_size;		}
	int capacity() const	{ return m_capacity;	}

	void resize(int newSize);
	
	void reserve(int n);
	
	bool inBound(int i) const	{ return i >=0 && i < m_size; }

	void append(const T& v);
	void push_back(const T& v) { append(v); }
	void operator+=(const T& v) { append(v); }

	void append(T&& v);
	void push_back(T&& v)	{ append(SGE_FORWARD(v)); }
	void operator+=(T&& v) { append(SGE_FORWARD(v)); }

	template<typename... ARGS>
	void emplace_back(ARGS&&... args);

private:
	void inBoundCheck(int i) const {
		if (!inBound(i)) {
			throw SGE_ERROR("Out of bound");
		}
	}

	T* m_data = nullptr;
	int	m_size = 0;
	int	m_capacity = 0;

}; // MyVector

template<class T> inline
void MyVector<T>::resize(int newSize)
{
	if (m_size == newSize) return;
	reserve(newSize);

	if (m_size < newSize) {
		auto* dst = m_data + m_size;
		auto* end = m_data + newSize;

		for (; dst < end; dst++) {
			new (dst) T();
		}
		m_size = newSize;
	}
}

template<class T> inline
void MyVector<T>::reserve(int n) {
	if (n <= m_capacity) {
		return;
	}

	int newCapacity = m_capacity + m_capacity / 2;
	if (newCapacity < n) {
		newCapacity = n;
	}

	auto* newData = reinterpret_cast<T*>(std::malloc(newCapacity * sizeof(T)));

	try {
		auto* src = m_data;
		auto* end = m_data + m_size;
		auto* dst = newData;

		for (; src < end; src++, dst++) {
			new(dst) T(std::move(*src));
			src->~T();
		}
	}
	catch (...) {
		std::free(newData);
		throw;
	}

	m_capacity = newCapacity;
	m_data = newData;
}

template<class T> inline
void MyVector<T>::append(const T& v) {
	reserve(m_size + 1);
	new (m_data + m_size) T(v);
	m_size++;
}

template<class T> inline
void MyVector<T>::append(T&& v) {
	reserve(m_size + 1);
	new (m_data + m_size) T(SGE_FORWARD(v));
	m_size++;
}

template<class T>
template<typename ...ARGS>
inline void MyVector<T>::emplace_back(ARGS && ...args)
{
	reserve(m_size + 1);
	new (m_data + m_size) T(SGE_FORWARD(args)...);
	m_size++;
}

} // namespace
