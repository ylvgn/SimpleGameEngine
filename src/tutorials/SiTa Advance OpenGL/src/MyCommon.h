#pragma once

#include <sge_core.h>

#include "sge_sita_advance_opengl-config.h"

namespace sge {

inline
int32_t my_next_pow2(int32_t v) { 
	if (v <= 0)
		return 0;

	v --;
	v |= v>>1;
	v |= v>>2;
	v |= v>>4;
	v |= v>>8;
	v |= v>>16;
	return v + 1;
}

inline
bool my_is_pow2(int v) {
	return !(v & (v - 1)) && v ;
}


#if SGE_OS_WINDOWS
class MyTimer {
public:
	MyTimer() { 
		reset(); 
	}

	void reset() {
		m_start = getTick();
	}

	double get() {
		auto c = getTick();
		return (c - m_start) / 1000.0f;
	}

private:

	uint64_t getTick() { 
		return GetTickCount64();
	}

	uint64_t m_start;
};

inline
uint64_t my_to_uint64(const LARGE_INTEGER& v) {
	return static_cast<uint64_t>(v.HighPart) << 32 | v.LowPart;
}

class MyHiResTimer : public NonCopyable {
public:
	MyHiResTimer() {
		::LARGE_INTEGER f;
		::QueryPerformanceFrequency(&f);
		_freq = my_to_uint64(f);
		reset();
	}

	void reset() {
		_start = _getTick();
	}

	double get() {
		auto c = _getTick();
		return static_cast<double>(c - _start) / _freq;
	}

private:
	uint64_t _getTick() {
		::LARGE_INTEGER v;
		::QueryPerformanceCounter(&v);
		return my_to_uint64(v);
	}

	uint64_t _freq;
	uint64_t _start;
};
#endif

}
